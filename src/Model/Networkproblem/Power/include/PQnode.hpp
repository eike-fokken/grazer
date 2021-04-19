#pragma once
#include <Powernode.hpp>

namespace Model::Networkproblem::Power {

  class PQnode final : public Powernode {

  public:
    static std::string get_type();
    std::string get_power_type() const override;
    using Powernode::Powernode;

    /// In this node  evaluate the equation
    /// for P and Q.
    virtual void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;

    void
    save_values(double time, Eigen::Ref<Eigen::VectorXd const> state) override;

    void json_save(
        nlohmann::json &output, double time,
        Eigen::Ref<Eigen::VectorXd const> state) const override;
  };

} // namespace Model::Networkproblem::Power
