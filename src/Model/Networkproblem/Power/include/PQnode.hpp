#pragma once
#include <Powernode.hpp>

namespace Model::Networkproblem::Power {

  class PQnode final : public Powernode {

  public:
    using Powernode::Powernode;

    /// In this node we set V to its boundary value and evaluate the equation
    /// for P.
    virtual void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                          double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                          Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;

    void evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                   double last_time, double new_time,
                                   Eigen::Ref<Eigen::VectorXd const> const &,
                                   Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;

    void display() const final;
  };

} // namespace Model::Networkproblem::Power
