#pragma once
#include <Eigen/Sparse>
#include <Matrixhandler.hpp>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

/*! \namespace Model
 *  This namespace holds all data for setting up model equations and for taking
 * derivatives thereof.
 */

namespace GrazerTest {

  using rootfunction = Eigen::VectorXd(Eigen::VectorXd);
  using Derivative = Eigen::SparseMatrix<double>(Eigen::VectorXd);

  class TestProblem {

  public:
    /// The constructor needs to declare Delta_t
    ///
    TestProblem(rootfunction _f, Derivative _df) : f(_f), df(_df){};

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

      rootvalues = f(new_state);
    };

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double, double,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

      Eigen::SparseMatrix<double> mat = df(new_state);
      for (int k = 0; k < mat.outerSize(); ++k)
        for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
          jacobianhandler->set_coefficient(
              static_cast<int>(it.row()), static_cast<int>(it.col()),
              it.value());
        }
    };

    rootfunction *f;
    Derivative *df;
  };
} // namespace GrazerTest
