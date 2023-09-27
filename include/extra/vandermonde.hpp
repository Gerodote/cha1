#ifndef EXTRA_VANDERMONDE_HPP
#define EXTRA_VANDERMONDE_HPP

#include <Eigen/Core>
#include <Eigen/Dense>
#include <iostream>

template <typename Scalar>
Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> create_vandermonde_matrix(Eigen::Matrix<Scalar, Eigen::Dynamic, 1> vector, Eigen::size_t order) {
  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> result(vector.rows(),order);
  for(Eigen::Index row = 0; row < result.rows(); ++row) {
    Scalar tmp = vector[row];
    auto tmp2 = tmp;
    result(row,0) = 1;
    for(Eigen::Index col = 1; col < result.cols(); ++col) {
      result(row,col) = tmp2;
      tmp2 *= tmp;
    }
  }
  return result;

};



#endif // !EXTRA_VANDERMONDE_HPP
