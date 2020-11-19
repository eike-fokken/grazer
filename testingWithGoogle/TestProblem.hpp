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

class TestProblem {

public:
  /// The constructor needs to declare Delta_t
  ///
  TestProblem(){};

  void evaluate(Eigen::VectorXd &rootfunction, double last_time,
                double new_time, Eigen::VectorXd const &last_state,
                Eigen::VectorXd const &new_state) {

    Eigen::Matrix2d A;
    A << 2, 1, 0, 3;
    Eigen::Vector2d b;
    b << 1, 0;

    rootfunction = A * new_state + b;
  };

  void evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                 double last_time, double new_time,
                                 Eigen::VectorXd const &last_state,
                                 Eigen::VectorXd const &new_state) {

    jacobianhandler->set_coefficient(0, 0, 2.);
    jacobianhandler->set_coefficient(0, 1, 1.);
    jacobianhandler->set_coefficient(1, 1, 3.);
  };
};
