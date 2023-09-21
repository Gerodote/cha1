#ifndef DATA_DATA_HPP_
#define DATA_DATA_HPP_

#include <boost/math/tools/polynomial.hpp>
#include <cstdint>
#include <ostream>

namespace data
{
  class DataGeneratorPolynome
  {
   public:
    explicit DataGeneratorPolynome(uint64_t order_polynome, double min_value_coefs, double max_value_coefs);
    double operator()(double point_at_which_evaluate_the_polynomial);

    friend std::ostream& operator<<(std::ostream& output, const DataGeneratorPolynome& poly_gen);

   private:
    uint64_t order_polynome_;
    double min_value_coefs_;
    double max_value_coefs_;
    boost::math::tools::polynomial<double> poly_;
  };
}  // namespace data

#endif  // DATA_DATA_HPP_
