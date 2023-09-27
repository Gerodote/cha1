#include <matplot/freestanding/axes_functions.h>
#include <matplot/freestanding/plot.h>
#include <matplot/matplot.h>
#include <matplot/util/keywords.h>

#include <algorithm>
#include <boost/math/tools/polynomial.hpp>
#include <cstdint>
#include <iostream>
#include <iterator>

#include "Eigen/Dense"
#include "data/generator_polynomial.hpp"
#include "extra/vandermonde.hpp"
#include "simdjson.h"

Eigen::Matrix<double, Eigen::Dynamic, 1> linear_least_square(
    const Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>& matrix,
    const Eigen::Matrix<double, Eigen::Dynamic, 1>& vector)
{
  auto smth = matrix.completeOrthogonalDecomposition();
  // https://eigen.tuxfamily.org/dox/classEigen_1_1CompleteOrthogonalDecomposition.html#ab5e8b3f2c7b602772e1f1d7ce63d446e
  return smth.solve(vector);
}

int main()
{
  simdjson::ondemand::parser parser;
  simdjson::padded_string config_init_data_json = simdjson::padded_string::load("config_init_data.json");
  simdjson::ondemand::document config_init_data = parser.iterate(config_init_data_json);

  auto left_boundary = static_cast<double>(config_init_data["left_boundary"]);
  auto right_boundary = static_cast<double>(config_init_data["right_boundary"]);
  auto quantity_points = static_cast<uint64_t>(config_init_data["quantity_points"]);

  Eigen::Matrix<double, 1, Eigen::Dynamic> x_axis(quantity_points);
  Eigen::Matrix<double, Eigen::Dynamic, 1> generated_data(quantity_points);

  uint64_t order_polynome;
  boost::math::tools::polynomial<double> polynome;
  simdjson::ondemand::array json_array_of_coefs;
  auto error = config_init_data["polynome_coefs"].get(json_array_of_coefs);
  if (error != simdjson::SUCCESS)
  {
    auto order_polynome_to_create = static_cast<uint64_t>(config_init_data["generator"]["polynome_generator"]["order_polynome"]);
    auto min_value_coefs = static_cast<double>(config_init_data["generator"]["polynome_generator"]["min_value_coefs"]);
    auto max_value_coefs = static_cast<double>(config_init_data["generator"]["polynome_generator"]["max_value_coefs"]);

    data::GeneratorPolynome<double> polynomial_data_generator(order_polynome_to_create, min_value_coefs, max_value_coefs);

    polynome = boost::math::tools::polynomial<double>(polynomial_data_generator());
  }
  else
  {
    std::vector<double> poly_coefs;
    
    for (auto&& val : json_array_of_coefs)
    {
      poly_coefs.push_back(val);
    }

    polynome = boost::math::tools::polynomial<double>(poly_coefs);
  }
  order_polynome = polynome.degree();

  std::generate(
      x_axis.begin(),
      x_axis.end(),
      [step = 0, &left_boundary, &right_boundary, &quantity_points]() mutable -> double
      { return left_boundary + (step++) * (right_boundary - left_boundary) / static_cast<double>(quantity_points - 1); });
  std::transform(
      x_axis.begin(),
      x_axis.end(),
      generated_data.begin(),
      [&polynome](const double& x_point) { return polynome(x_point); });
  std::cout << "x-axis:\n" << x_axis << '\n';
  std::cout << "polynome coefs:\n" << polynome << '\n';
  std::cout << "generated data:\n" << generated_data.transpose() << '\n';

  auto vandermonde = create_vandermonde_matrix<double>(x_axis, order_polynome + 1);

  std::cout << "vandermonde:\n" << vandermonde << '\n';
  auto solution = linear_least_square(vandermonde, generated_data);

  std::cout << "solution:\n" << solution << "\n";

  std::vector<double> solution_vector(solution.data(), solution.data() + solution.size());
  boost::math::tools::polynomial<double> approx_poly(solution_vector);
  std::vector<double> approx_data_vector;
  approx_data_vector.reserve(quantity_points);
  std::transform(
      x_axis.begin(),
      x_axis.end(),
      std::back_inserter(approx_data_vector),
      [&approx_poly](const double& x_point) { return approx_poly(x_point); });

  std::cout << "\napprox_data_vector:\n";
  for (const auto& val : approx_data_vector)
  {
    std::cout << val << ' ';
  }
  std::cout << "\n";
  std::vector<double> generated_data_vector(generated_data.data(), generated_data.data() + generated_data.size());

  auto plot1 = matplot::plot(x_axis, generated_data_vector);
  plot1->display_name("generated data");
  plot1->line_width(5);
  matplot::hold(matplot::on);

  auto plot2 = matplot::plot(x_axis, approx_data_vector);
  plot2->display_name("approximation of the data");
  plot2->line_width(3);
  matplot::hold(matplot::off);

  matplot::legend({});

  matplot::show();
}
