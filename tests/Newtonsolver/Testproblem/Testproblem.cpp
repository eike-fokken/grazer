#include "TestProblem.hpp"
#include "Matrixhandler.hpp"

TestProblem::TestProblem(rootfunction _f, Derivative _df) : f(_f), df(_df) {}

void TestProblem::evaluate(
    Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
    Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const {

  rootvalues = f(new_state);
}

void TestProblem::d_evaluate_d_new_state(
    Aux::Matrixhandler &jacobianhandler, double, double,
    Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const {

  Eigen::SparseMatrix<double> mat = df(new_state);
  for (int k = 0; k < mat.outerSize(); ++k)
    for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
      jacobianhandler.add_to_coefficient(
          static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
    }
}
