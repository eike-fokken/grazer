#include "TestProblem.hpp"
#include "Matrixhandler.hpp"

namespace GrazerTest {

  TestProblem::TestProblem(rootfunction _f, Derivative _df) : f(_f), df(_df) {}

  void TestProblem::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {

    rootvalues = f(new_state);
  }

  void TestProblem::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {

    Eigen::SparseMatrix<double> mat = df(new_state);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        jacobianhandler->set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  }

} // namespace GrazerTest
