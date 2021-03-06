#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <memory>
#include <vector>

namespace Aux {
  class Matrixhandler;
}
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
    TestProblem(rootfunction _f, Derivative _df);

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const;

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double, double,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const;

    rootfunction *f;
    Derivative *df;
  };
} // namespace GrazerTest
