#include "Matrixhandler.hpp"

#include <Eigen/Sparse>
#include <Eigen/src/SparseCore/SparseMatrix.h>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest-death-test.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Aux;

TEST(Triplethandler, add_to_coefficient) {

  Eigen::SparseMatrix<double> mat(5, 5);
  Eigen::MatrixXd expected_mat(5, 5);
  Triplethandler handler(mat);
  double value = 1.0;
  for (Eigen::Index col = 0; col != mat.cols(); ++col) {
    for (Eigen::Index row = 0; row != mat.rows(); ++row) {
      handler.add_to_coefficient(row, col, value);
      expected_mat(row, col) = value;
      ++value;
    }
  }
  handler.set_matrix();
  Eigen::MatrixXd dense = mat;
  EXPECT_EQ(expected_mat, dense);
}

#ifndef NDEBUG
TEST(TriplethandlerDeathTest, assert_empty_matrix) {
  GTEST_FLAG_SET(death_test_style, "threadsafe");
  Eigen::SparseMatrix<double> mat(5, 5);
  Eigen::MatrixXd expected_mat(5, 5);
  Triplethandler handler(mat);
  double value = 1.0;
  for (Eigen::Index col = 0; col != mat.cols(); ++col) {
    for (Eigen::Index row = 0; row != mat.rows(); ++row) {
      handler.add_to_coefficient(row, col, value);
      expected_mat(row, col) = value;
      ++value;
    }
  }
  handler.set_matrix();
  EXPECT_DEATH(Triplethandler h2(mat), "nonZeros().*==.*0");
}
#endif
TEST(Triplethandler, add_to_coefficient2) {

  Eigen::SparseMatrix<double> mat(5, 5);
  Triplethandler handler(mat);
  for (Eigen::Index col = 0; col != mat.cols(); ++col) {
    for (Eigen::Index row = 0; row != mat.rows(); ++row) {
      handler.add_to_coefficient(row, col, 6.0);
    }
  }

  Eigen::MatrixXd expected_mat(5, 5);

  double value = 1.0;
  for (Eigen::Index col = 0; col != mat.cols(); ++col) {
    for (Eigen::Index row = 0; row != mat.rows(); ++row) {
      handler.add_to_coefficient(row, col, value);
      expected_mat(row, col) = value + 6.0;
      ++value;
    }
  }
  handler.set_matrix();
  Eigen::MatrixXd dense = mat;
  EXPECT_EQ(expected_mat, dense);
}

TEST(Coeffrefhandler, add_to_coefficient) {

  Eigen::SparseMatrix<double> mat(5, 5);

  // prepare layout:
  Triplethandler triplethandler(mat);
  for (Eigen::Index col = 0; col != mat.cols(); ++col) {
    for (Eigen::Index row = 0; row != mat.rows(); ++row) {
      triplethandler.add_to_coefficient(row, col, 0.0);
    }
  }
  triplethandler.set_matrix();

  Eigen::MatrixXd expected_mat(5, 5);
  Coeffrefhandler coeffrefhandler(mat);
  double value = 1.0;
  for (Eigen::Index col = 0; col != mat.cols(); ++col) {
    for (Eigen::Index row = 0; row != mat.rows(); ++row) {
      coeffrefhandler.add_to_coefficient(row, col, value);
      expected_mat(row, col) = value;
      ++value;
    }
  }
  coeffrefhandler.set_matrix();
  Eigen::MatrixXd dense = mat;
  EXPECT_EQ(expected_mat, dense);
}

TEST(Triplethandler_transposed, add_to_coefficient) {

  Eigen::SparseMatrix<double> mat(5, 5);
  Eigen::SparseMatrix<double> mat_transposed(5, 5);

  Triplethandler handler(mat);
  Triplethandler<1> transposedhandler(mat_transposed);
  double value = 1.0;
  for (Eigen::Index col = 0; col != mat.cols(); ++col) {
    for (Eigen::Index row = 0; row != mat.rows(); ++row) {
      handler.add_to_coefficient(row, col, value);
      transposedhandler.add_to_coefficient(row, col, value);
      ++value;
    }
  }
  handler.set_matrix();
  transposedhandler.set_matrix();

  Eigen::MatrixXd compare_mat = mat.transpose();
  Eigen::MatrixXd compare_mat_transposed = mat_transposed;

  EXPECT_EQ(compare_mat, compare_mat_transposed);
}

TEST(Coeffrefhandler_transposed, add_to_coefficient) {

  Eigen::SparseMatrix<double> mat(5, 5);
  Eigen::SparseMatrix<double> mat_transposed(5, 5);

  {
    Triplethandler handler(mat);
    Triplethandler<1> transposedhandler(mat_transposed);
    for (Eigen::Index col = 0; col != mat.cols(); ++col) {
      for (Eigen::Index row = 0; row != mat.rows(); ++row) {
        handler.add_to_coefficient(row, col, 0.0);
        transposedhandler.add_to_coefficient(row, col, 0.0);
      }
    }
    handler.set_matrix();
    transposedhandler.set_matrix();
  }

  Coeffrefhandler coeffrefhandler(mat);
  Coeffrefhandler<Transposed> transposedcoeffrefhandler(mat_transposed);
  double value = 1.0;
  for (Eigen::Index col = 0; col != mat.cols(); ++col) {
    for (Eigen::Index row = 0; row != mat.rows(); ++row) {
      coeffrefhandler.add_to_coefficient(row, col, value);
      transposedcoeffrefhandler.add_to_coefficient(row, col, value);
      ++value;
    }
  }
  coeffrefhandler.set_matrix();
  transposedcoeffrefhandler.set_matrix();

  Eigen::MatrixXd compare_mat = mat.transpose();
  Eigen::MatrixXd compare_mat_transposed = mat_transposed;

  EXPECT_EQ(compare_mat, compare_mat_transposed);
}
