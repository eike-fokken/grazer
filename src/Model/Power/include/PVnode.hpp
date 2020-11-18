#pragma once
#include <Powernode.hpp>

namespace Aux {

  class Matrixhandler;
}

namespace Model::Networkproblem::Power {

  class PVnode final : public Powernode {

  public:
    using Powernode::Powernode;

    /// In this node we set V to its boundary value and evaluate the equation
    /// for P.
    void evaluate(Eigen::VectorXd &rootfunction, double last_time,
                  double new_time, Eigen::VectorXd const &last_state,
                  Eigen::VectorXd const &new_state) final;

    void evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                   double last_time, double new_time,
                                   Eigen::VectorXd const &,
                                   Eigen::VectorXd const &new_state) final;

    void display() const override;
  };

} // namespace Model::Networkproblem::Power
