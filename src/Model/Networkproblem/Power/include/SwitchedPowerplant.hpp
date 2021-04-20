#pragma once
#include "Boundaryvalue.hpp"
#include "Control.hpp"
#include "Powernode.hpp"

namespace Model::Networkproblem::Power {

  class SwitchedPowerplant final : public Powernode {

  public:
    static std::string get_type();
    std::string get_power_type() const override;
    static nlohmann::json get_schema();

    SwitchedPowerplant(nlohmann::json const &topology);

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

    void json_save(
        nlohmann::json &output, double time,
        Eigen::Ref<Eigen::VectorXd const> state) const override;

  private:
    // Problem: We also need to switch out the boundary values!
    // Carful: We always need to introduce 2 times to make sure that the value
    // is actually 0!
    Control<1> is_PV_node;
    Boundaryvalue<2> boundary_values_pv;
  };

} // namespace Model::Networkproblem::Power
