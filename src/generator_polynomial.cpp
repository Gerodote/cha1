#include "data/generator_polynomial.hpp"

#include <algorithm>
#include <boost/math/tools/polynomial.hpp>
#include <iterator>
#include <random>
#include <vector>

namespace data
{
  DataGeneratorPolynome::DataGeneratorPolynome(uint64_t order_polynome, double min_value_coefs, double max_value_coefs)
      : order_polynome_(order_polynome),
        min_value_coefs_(min_value_coefs),
        max_value_coefs_(max_value_coefs)
  {
    std::random_device pure_random;
    std::mt19937_64 random_gen(pure_random());
    std::uniform_real_distribution<double> dis(min_value_coefs_, max_value_coefs_);
    std::vector<double> coefs;
    coefs.reserve(order_polynome_ + 1);
    std::generate_n(std::back_inserter(coefs), order_polynome_ + 1, [&random_gen, &dis]() { return dis(random_gen); });
    poly_ = boost::math::tools::polynomial<double>(coefs);
  }

  double DataGeneratorPolynome::operator()(double point_at_which_evaluate_the_polynomial)
  {
    return poly_(point_at_which_evaluate_the_polynomial);
  }

  std::ostream& operator<<(std::ostream& output, const DataGeneratorPolynome& poly_gen)
  {
    return output << poly_gen.poly_;
  }

}  // namespace data
