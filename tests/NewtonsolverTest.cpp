#include "TestProblem.hpp"
#include <Matrixhandler.hpp>
#include <Newtonsolver.hpp>
#include <gtest/gtest.h>
#include <iostream>

Eigen::VectorXd f(Eigen::VectorXd x);
Eigen::VectorXd f2(Eigen::VectorXd x);
Eigen::SparseMatrix<double> df(Eigen::VectorXd);
Eigen::SparseMatrix<double> df2(Eigen::VectorXd);

Eigen::VectorXd f(Eigen::VectorXd x) {
  Eigen::Matrix2d A;
  A << 2, 1, 0, 3;
  Eigen::Vector2d b;
  b << 1, 0;
  return A * x + b;
}

Eigen::VectorXd f2(Eigen::VectorXd x) {
  Eigen::Matrix2d A;
  A << 1, 1, 0, 1;
  Eigen::Vector2d b, y;
  y(0) = x(0) * x(0);
  y(1) = x(1) * x(1);
  b << 9, 0;

  return (A * y + b);
}

Eigen::SparseMatrix<double> df(Eigen::VectorXd) {

  Eigen::Matrix2d A;
  A << 2, 1, 0, 3;
  return A.sparseView();
}

Eigen::SparseMatrix<double> df2(Eigen::VectorXd x) {

  Eigen::Matrix2d A;
  A << 2 * x[0], 2 * x[1], 0.0, 2 * x[1];
  return A.sparseView();
}

TEST(Newtonsolver, LinearSolveWithRoot_InitialValue1) {
  double tol = 1e-12;
  int max_it = 10000;

  Solver::Newtonsolver_test Solver(tol, max_it);
  Eigen::VectorXd new_state(2), last_state(2), solution(2);
  new_state(0) = 5; // Wähle Funktionswert, der etwas weiter weg liegt
  new_state(1) = 3;

  last_state(0) = 0;
  last_state(1) = 0;

  solution(0) = -0.5;
  solution(1) = 0.;

  double last_time = 0;
  double new_time = 1;

  GrazerTest::TestProblem problem(f, df);
  Solver::Solutionstruct a;

  a = Solver.solve(new_state, problem, true, last_time, new_time, last_state);

  EXPECT_EQ(a.success, true);

  EXPECT_DOUBLE_EQ(new_state(0), solution(0));
  EXPECT_DOUBLE_EQ(new_state(1), solution(1));
}

TEST(Newtonsolver, LinearSolveWithRoot_InitialValue2) {
  double tol = 1e-12;
  int max_it = 10000;

  Solver::Newtonsolver_test Solver(tol, max_it);
  Eigen::VectorXd new_state(2), last_state(2), solution(2);
  new_state(0) = -0.5; // Wähle Funktionswert, der nah an der Lösung ist
  new_state(1) = 0.;

  last_state(0) = 0;
  last_state(1) = 0;

  solution(0) = -0.5;
  solution(1) = 0.;

  double last_time = 0;
  double new_time = 1;

  GrazerTest::TestProblem problem(f, df);
  Solver::Solutionstruct a;

  a = Solver.solve(new_state, problem, true, last_time, new_time, last_state);

  EXPECT_EQ(a.success, true); // passed

  // schlägt fehl
  EXPECT_DOUBLE_EQ(new_state(0), solution(0));
  EXPECT_DOUBLE_EQ(new_state(1), solution(1));
}

TEST(Newtonsolver, NonlinearSolveWithRoot) {
  double tol = 1e-12;
  int max_it = 100;

  Solver::Newtonsolver_test Solver(tol, max_it);
  Eigen::VectorXd new_state(2), last_state(2), solution(2);

  last_state(0) = 0;
  last_state(1) = 0;

  new_state(0) = 1;
  new_state(1) = 1;

  solution(0) = 3;
  solution(1) = 0.;

  double last_time = 0;
  double new_time = 1;

  GrazerTest::TestProblem problem(f2, df2);
  Solver::Solutionstruct a;
  a = Solver.solve(new_state, problem, true, last_time, new_time, last_state);

  EXPECT_EQ(a.success, false);
  EXPECT_EQ(a.used_iterations, max_it);
}

TEST(Newtonsolver, SingularJacobian) {
  double tol = 1e-12;
  int max_it = 100;

  Solver::Newtonsolver_test Solver(tol, max_it);
  Eigen::VectorXd new_state(2), last_state(2), solution(2);

  last_state(0) = 0;
  last_state(1) = 0;

  new_state = last_state;

  solution(0) = 3;
  solution(1) = 0.;

  double last_time = 0;
  double new_time = 1;

  GrazerTest::TestProblem problem(f2, df2);
  Solver::Solutionstruct a;

  EXPECT_ANY_THROW(
      a = Solver.solve(
          new_state, problem, true, last_time, new_time, last_state););
}