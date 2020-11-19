#include "TestProblem.hpp"
#include <Matrixhandler.hpp>
#include <Newtonsolver.hpp>
#include <gtest/gtest.h>
#include <iostream>

Eigen::VectorXd f(Eigen::VectorXd x) {
  Eigen::Matrix2d A;
  A << 2, 1, 0, 3;
  Eigen::Vector2d b;
  b << 1, 0;
  return A * x + b;
};

Eigen::SparseMatrix<double> df(Eigen::VectorXd x) {

  Eigen::Matrix2d A;
  A << 2, 1, 0, 3;
  return A.sparseView();
};

TEST(Newtonsolver, SolveWithRoot) {
  double tol = 1e-12;
  int max_it = 10000;

  Solver::Newtonsolver<TestProblem> Solver(tol, max_it);
  bool new_jacobian_structure = true;
  Eigen::VectorXd new_state(2), last_state(2), solution(2);
  new_state(0) = 5; // Wähle Funktionswert, der weit weg ist
  new_state(1) = 3;

  last_state(0) = 0;
  last_state(1) = 0;

  solution(0) = -0.5;
  solution(1) = 0.;

  double last_time = 0;
  double new_time = 1;

  TestProblem problem(f, df);
  Solver::Solutionstruct a;

  a = Solver.solve(new_state, problem, new_jacobian_structure, last_time,
                   new_time, last_state);

  EXPECT_EQ(a.success, true);

  EXPECT_DOUBLE_EQ(new_state(0), solution(0));
  EXPECT_DOUBLE_EQ(new_state(1), solution(1));
}
