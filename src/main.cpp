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
  Eigen::MatrixXd pseudo_inverse = matrix.completeOrthogonalDecomposition().pseudoInverse();
  return pseudo_inverse * vector;
}

int main()
{
  simdjson::ondemand::parser parser;
  simdjson::padded_string config_init_data_json = simdjson::padded_string::load("config_init_data.json");
  simdjson::ondemand::document config_init_data = parser.iterate(config_init_data_json);

  auto order_polynome = static_cast<uint64_t>(config_init_data["generator"]["polynome_generator"]["order_polynome"]);
  auto min_value_coefs = static_cast<double>(config_init_data["generator"]["polynome_generator"]["min_value_coefs"]);
  auto max_value_coefs = static_cast<double>(config_init_data["generator"]["polynome_generator"]["max_value_coefs"]);

  data::DataGeneratorPolynome polynomial_data_generator(order_polynome, min_value_coefs, max_value_coefs);

  auto left_boundary = static_cast<double>(config_init_data["generator"]["left_boundary"]);
  auto right_boundary = static_cast<double>(config_init_data["generator"]["right_boundary"]);
  auto quantity_points = static_cast<uint64_t>(config_init_data["generator"]["quantity_points"]);

  Eigen::Matrix<double, 1, Eigen::Dynamic> x_axis(quantity_points);
  Eigen::Matrix<double, Eigen::Dynamic, 1> generated_data(quantity_points);

  std::generate(
      x_axis.begin(),
      x_axis.end(),
      [step = 0, &left_boundary, &right_boundary, &quantity_points]() mutable -> double
      { return left_boundary + (step++) * (right_boundary - left_boundary) / static_cast<double>(quantity_points - 1); });
  std::transform(
      x_axis.begin(),
      x_axis.end(),
      generated_data.begin(),
      [&polynomial_data_generator](const double& x_point) { return polynomial_data_generator(x_point); });
  std::cout << "x-axis:\n" << x_axis << '\n';
  std::cout << "polynome coefs:\n" << polynomial_data_generator << '\n';
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
  

  // std::ifstream fin_matrix;
  // fin_matrix.open("matrix.txt");
  // uint64_t rows = 0;
  // uint64_t cols = 0;
  // fin_matrix >> rows >> cols;
  // Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> matrix_to_solve(rows, cols);
  // double a_value = 0;
  // for (uint64_t row = 0; row < rows; ++row)
  // {
  //   for (uint64_t col = 0; col < cols; ++col)
  //   {
  //     fin_matrix >> a_value;
  //     matrix_to_solve(static_cast<Eigen::Index>(row), static_cast<Eigen::Index>(col)) = a_value;
  //   }
  // }
  //
  // std::cout << "matrix_to_solve:\n" << matrix_to_solve << '\n';
  //
  // std::ifstream fin_vector;
  // fin_vector.open("vector.txt");
  //
  // Eigen::Matrix<double,Eigen::Dynamic,1,0,Eigen::Dynamic,1> a_vector(rows);
  //
  // for (auto& row : a_vector)
  // {
  //   fin_vector >> row;
  // }
  //
  // std::cout << "vector b:\n" << a_vector << '\n';
  //
  // Eigen::MatrixXd pseudo_inverse_of_matrix_to_solve = matrix_to_solve.completeOrthogonalDecomposition().pseudoInverse();
  //
  // std::cout << "pseudo_inverse_of_matrix_to_solve:\n" << pseudo_inverse_of_matrix_to_solve << '\n';
  //
  // std::cout << "Checking if the only solution exists:\n";
  //
  // auto maybe_a_solution = pseudo_inverse_of_matrix_to_solve * a_vector;
  // std::cout << "A*(A^+)*b = \n" <<  matrix_to_solve * pseudo_inverse_of_matrix_to_solve * a_vector << "\n";
  // if ((matrix_to_solve * pseudo_inverse_of_matrix_to_solve * a_vector).isApprox(a_vector))
  // {
  //   std::cout << "yes\nSolution is:\n" << maybe_a_solution << '\n';
  // }
  // else
  // {
  //   std::cout << "no\nSet of solution can be described as:\n" << maybe_a_solution << "\n+\n" <<
  //   Eigen::MatrixXd::Identity(static_cast<Eigen::Index>(rows),static_cast<Eigen::Index>(rows)) - matrix_to_solve *
  //   pseudo_inverse_of_matrix_to_solve << " * any_vector_appropriate_size\n";
  //
  // }
  //
  // fin_matrix.close();
  // fin_vector.close();
}
