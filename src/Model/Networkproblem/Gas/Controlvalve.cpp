#include "Controlvalve.hpp"
#include "Gasedge.hpp"
#include "Get_base_component.hpp"
#include "Matrixhandler.hpp"
#include "Shortcomponent.hpp"
#include "make_schema.hpp"
#include <fstream>
#include <iostream>

namespace Model::Networkproblem::Gas {
  std::string Controlvalve::get_type() { return "Controlvalve"; }
  std::string Controlvalve::get_gas_type() const { return get_type(); }

  std::optional<nlohmann::json> Controlvalve::get_control_schema() {
    return Aux::schema::make_boundary_schema(1);
  }

  Controlvalve::Controlvalve(
      nlohmann::json const &data,
      std::vector<std::unique_ptr<Network::Node>> &nodes) :
      Shortcomponent(data, nodes), control_values(data["control_values"]) {}

  void Controlvalve::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state,
      Eigen::Ref<Eigen::VectorXd const> /*last_control*/,
      Eigen::Ref<Eigen::VectorXd const> /*new_control*/) const {

    Eigen::Vector2d pressure_control(control_values(new_time)[0], 0.0);
    rootvalues.segment<2>(get_equation_start_index())
        = get_boundary_state(1, new_state) - get_boundary_state(-1, new_state)
          - pressure_control;
  }

  void Controlvalve::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double /*last_time*/,
      double /*new_time*/, Eigen::Ref<Eigen::VectorXd const> /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> /*new_state*/,
      Eigen::Ref<Eigen::VectorXd const> /*last_control*/,
      Eigen::Ref<Eigen::VectorXd const> /*new_control*/) const {
    auto start_p_index = get_boundary_state_index(1);
    auto start_q_index = start_p_index + 1;
    auto end_p_index = get_boundary_state_index(-1);
    auto end_q_index = end_p_index + 1;

    auto start_equation_index = get_equation_start_index();
    auto end_equation_index = start_equation_index + 1;

    jacobianhandler->set_coefficient(start_equation_index, start_p_index, 1.0);
    jacobianhandler->set_coefficient(start_equation_index, end_p_index, -1.0);
    jacobianhandler->set_coefficient(end_equation_index, start_q_index, 1.0);
    jacobianhandler->set_coefficient(end_equation_index, end_q_index, -1.0);
  }

  void Controlvalve::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) {
    initial_values_helper(new_state, initial_json);
  }

  void Controlvalve::add_results_to_json(nlohmann::json &new_output) {
    std::string comp_type = Aux::component_class(*this);
    new_print_helper(new_output, comp_type, get_type());
  }

} // namespace Model::Networkproblem::Gas
