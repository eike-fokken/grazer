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

using Rootfunction = Eigen::VectorXd(Eigen::VectorXd);
using Derivative = Eigen::SparseMatrix<double>(Eigen::VectorXd);

class TestProblem {

public:
  /// The constructor needs to declare Delta_t
  ///
  TestProblem(Rootfunction _f, Derivative _df) : f(_f), df(_df){};

  void evaluate(Eigen::VectorXd &rootfunction, double, double,
                Eigen::VectorXd const &, Eigen::VectorXd const &new_state) const{

    rootfunction = f(new_state);
  };

  void evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler, double,
                                 double, Eigen::VectorXd const &,
                                 Eigen::VectorXd const &new_state) const{

    Eigen::SparseMatrix<double> mat = df(new_state);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        jacobianhandler->set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  };

  Rootfunction *f;
  Derivative *df;
};
}
