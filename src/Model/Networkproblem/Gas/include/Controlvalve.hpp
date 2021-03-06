#pragma once
#include "Control.hpp"
#include "Edge.hpp"
#include "Gasedge.hpp"
#include "Shortcomponent.hpp"
#include <nlohmann/json.hpp>

namespace Model::Networkproblem::Gas {

  class Controlvalve final : public Shortcomponent {
  public:
    static std::string get_type();
    std::string get_gas_type() const override;

    static std::optional<nlohmann::json> get_control_schema();

    Controlvalve(
        nlohmann::json const &edge_json,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;
    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) override;

    void print_to_files(nlohmann::json &new_output) final;

  private:
    Control<1> const control_values;
  };

} // namespace Model::Networkproblem::Gas
