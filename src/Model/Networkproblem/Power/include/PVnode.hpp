#pragma once
#include <Powernode.hpp>

namespace Model::Networkproblem::Power {

  class PVnode final : public Powernode {

  public:
    static std::string get_type();
    static bool needs_boundary_values();

    using Powernode::Powernode;

    /// In this node we set V to its boundary value and evaluate the equation
    /// for P.
    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;

    void
    save_values(double time, Eigen::Ref<Eigen::VectorXd const> state) override;
  };

} // namespace Model::Networkproblem::Power
