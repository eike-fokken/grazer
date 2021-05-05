#pragma once
#include <Powernode.hpp>

namespace Model::Networkproblem::Power {

  class Vphinode final : public Powernode {

  public:
    static std::string get_type();
    std::string get_power_type() const override;

    using Powernode::Powernode;

    /// In this node we just set V and phi to their respective boundary values.
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

    void
    json_save(double time, Eigen::Ref<Eigen::VectorXd const> state) override;
  };

} // namespace Model::Networkproblem::Power
