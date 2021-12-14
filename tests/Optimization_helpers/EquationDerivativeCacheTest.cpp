#include "EquationDerivativeCache.hpp"
#include "Controlcomponent.hpp"
#include "InterpolatingVector.hpp"
#include "Mock_Controlcomponent.hpp"
#include "test_io_helper.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace testing;

static Eigen::VectorXd
f(Eigen::VectorXd last_state, Eigen::VectorXd new_state,
  Eigen::VectorXd control) {
  return new_state - last_state - control;
}

static Eigen::SparseMatrix<double>
df(Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd) {
  Eigen::SparseMatrix<double> A(2, 2);
  A.setIdentity();
  return A;
}

TEST(EquationDerivativeCache, happy) {
  Optimization::EquationDerivativeCache cache(1);
}
