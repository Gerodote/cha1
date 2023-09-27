#ifndef DATA_DATA_HPP_
#define DATA_DATA_HPP_

#include <boost/math/tools/polynomial.hpp>
#include <cstdint>
#include <ostream>
#include <random>

namespace data
{
  template<typename T>
  class GeneratorPolynome;  // forward declare to make function declaration possible
}

template<typename T>  // declaration
std::ostream& operator<<(std::ostream& output_stream, const data::GeneratorPolynome<T>& a_poly_gen);

namespace data
{
  template<typename T_float>
  class GeneratorPolynome
  {
   public:
    explicit GeneratorPolynome(uint64_t order_polynome, T_float min_value_coefs, T_float max_value_coefs)
        : order_polynome_(order_polynome),
          min_value_coefs_(min_value_coefs),
          max_value_coefs_(max_value_coefs),
          random_gen(std::random_device()()),
          dis(min_value_coefs, max_value_coefs)
    {
    }

    boost::math::tools::polynomial<T_float> operator()() const
    {
      std::vector<T_float> coefs;
      coefs.reserve(order_polynome_ + 1);
      std::generate_n(std::back_inserter(coefs), order_polynome_ + 1, [this]() { return this->dis(this->random_gen); });
      return std::move(boost::math::tools::polynomial<T_float>(std::move(coefs)));
    };

    friend std::ostream& operator<< <>(std::ostream& output, const GeneratorPolynome& poly_gen);

   private:
    uint64_t order_polynome_;
    T_float min_value_coefs_;
    T_float max_value_coefs_;
    mutable std::mt19937_64 random_gen;
    mutable std::uniform_real_distribution<T_float> dis;
  };

}  // namespace data

template<typename T_float>
std::ostream& operator<<(std::ostream& output, const data::GeneratorPolynome<T_float>& poly_gen)
{
  return output << poly_gen();
}
#endif  // DATA_DATA_HPP_
