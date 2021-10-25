#include "Optimization_helpers.hpp"
#include <gtest/gtest.h>
#include <vector>

TEST(AdjoinEquationsSolver, system1) {
  // Set up problem data
  std::vector<Eigen::SparseMatrix<double>> AT_vector(3);
  std::vector<Eigen::SparseMatrix<double>> BT_vector(3);
  std::vector<Eigen::VectorXd> df_dx_vector(3);

  // left hand side
  Eigen::Matrix<double, 3, 3> A1t;
  Eigen::Matrix<double, 3, 3> A2t;
  Eigen::Matrix<double, 3, 3> B1t;
  Eigen::Matrix<double, 3, 3> B2t;
  A1t << 2, 5, 6, 1, 1, 6, 5, 9, 8;
  A2t << 4, 1, 7, 2, 4, 9, 0, 4, 9;
  B1t << 5, 4, 0, 7, 3, 8, 9, 1, 0;
  B2t << 8, 4, 0, 2, 2, 0, 5, 2, 4;
  AT_vector[1] = Eigen::SparseMatrix<double>(A1t.sparseView());
  AT_vector[2] = Eigen::SparseMatrix<double>(A2t.sparseView());
  BT_vector[1] = Eigen::SparseMatrix<double>(B1t.sparseView());
  BT_vector[2] = Eigen::SparseMatrix<double>(B2t.sparseView());

  // right hand side
  Eigen::VectorXd df_dx_f1(3);
  Eigen::VectorXd df_dx_f2(3);
  Eigen::VectorXd df_dx_f3(3);
  df_dx_f1 << 0.5, 0.5, 0.5;
  df_dx_f2 << 0.5, 0.5, 0.5;
  df_dx_f3 << 0.5, 0.5, 0.5;
  df_dx_vector[0] = df_dx_f1;
  df_dx_vector[1] = df_dx_f2;
  df_dx_vector[2] = df_dx_f3;

  // solution vector
  std::vector<Eigen::VectorXd> multipliers(3);

  // solver objects
  std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> solvers(3);

  // solve the adjoint equations system
  Optimization::initialize_solvers(BT_vector, solvers);
  Optimization::compute_multiplier(
      multipliers, AT_vector, BT_vector, df_dx_vector, solvers);

  EXPECT_NEAR(multipliers[0][0], -0.13256048, 1.0e-5);
  EXPECT_NEAR(multipliers[0][1], -1.04889113, 1.0e-5);
  EXPECT_NEAR(multipliers[0][2], 0.10987903, 1.0e-5);
  EXPECT_NEAR(multipliers[1][0], 0.23689516, 1.0e-5);
  EXPECT_NEAR(multipliers[1][1], -0.28830645, 1.0e-5);
  EXPECT_NEAR(multipliers[1][2], 0.1000504, 1.0e-5);
  EXPECT_NEAR(multipliers[2][0], 0.125, 1.0e-5);
  EXPECT_NEAR(multipliers[2][1], -0.375, 1.0e-5);
  EXPECT_NEAR(multipliers[2][2], -0.09375, 1.0e-5);
}
