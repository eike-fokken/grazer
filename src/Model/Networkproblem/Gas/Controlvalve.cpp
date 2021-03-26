#include "Controlvalve.hpp"
#include "Gasedge.hpp"
#include "Matrixhandler.hpp"
#include "Shortcomponent.hpp"
#include "make_schema.hpp"
#include <fstream>
#include <iostream>

namespace Model::Networkproblem::Gas {
  std::string Controlvalve::get_type() { return "Controlvalve"; }

  nlohmann::json Controlvalve::get_schema() {
    nlohmann::json schema = Shortcomponent::get_schema();
    Aux::schema::add_property(
        schema, "control_values", Aux::schema::make_boundary_schema(1));
    return schema;
  }

  Controlvalve::Controlvalve(
      nlohmann::json const &data,
      std::vector<std::unique_ptr<Network::Node>> &nodes) :
      Shortcomponent(data, nodes), control_values(data["control_values"]) {}

  void Controlvalve::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {

    Eigen::Vector2d pressure_control(control_values(new_time)[0], 0.0);
    rootvalues.segment<2>(get_equation_start_index())
        = get_boundary_state(1, new_state) - get_boundary_state(-1, new_state)
          - pressure_control;
  }

  void Controlvalve::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const>) const {
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
      nlohmann::ordered_json initial_json) {
    initial_values_helper(new_state, initial_json);
  }

  void
  Controlvalve::print_to_files(std::filesystem::path const &output_directory) {
    print_helper(output_directory, get_type());
  }

} // namespace Model::Networkproblem::Gas
