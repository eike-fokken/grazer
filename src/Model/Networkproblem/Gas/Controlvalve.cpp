/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#include "Controlvalve.hpp"
#include "Controlcomponent.hpp"
#include "Gasedge.hpp"
#include "Get_base_component.hpp"
#include "Matrixhandler.hpp"
#include "Shortcomponent.hpp"
#include "make_schema.hpp"

namespace Model::Gas {
  std::string Controlvalve::get_type() { return "Controlvalve"; }
  std::string Controlvalve::get_gas_type() const { return get_type(); }

  nlohmann::json Controlvalve::get_control_schema() {
    return Aux::schema::make_boundary_schema(1);
  }

  Controlvalve::Controlvalve(
      nlohmann::json const &data,
      std::vector<std::unique_ptr<Network::Node>> &nodes) :
      Shortcomponent(data, nodes) {}

  void Controlvalve::setup() { setup_output_json_helper(get_id()); }

  void Controlvalve::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double /*new_time*/,
      const Eigen::Ref<const Eigen::VectorXd> & /*last_state*/,
      const Eigen::Ref<const Eigen::VectorXd> &new_state,
      const Eigen::Ref<const Eigen::VectorXd> &control) const {

    Eigen::Vector2d pressure_control(control[get_control_startindex()], 0.0);

    rootvalues.segment<2>(get_equation_start_index())
        = get_boundary_state(end, new_state)
          - get_boundary_state(start, new_state) + pressure_control;
  }

  void Controlvalve::d_evaluate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double /*last_time*/,
      double /*new_time*/,
      const Eigen::Ref<const Eigen::VectorXd> & /*last_state*/,
      const Eigen::Ref<const Eigen::VectorXd> & /*new_state*/,
      const Eigen::Ref<const Eigen::VectorXd> & /*control*/) const {
    auto start_p_index = get_boundary_state_index(start);
    auto start_q_index = start_p_index + 1;
    auto end_p_index = get_boundary_state_index(end);
    auto end_q_index = end_p_index + 1;

    auto start_equation_index = get_equation_start_index();
    auto end_equation_index = start_equation_index + 1;

    jacobianhandler.add_to_coefficient(
        start_equation_index, start_p_index, -1.0);
    jacobianhandler.add_to_coefficient(start_equation_index, end_p_index, 1.0);
    jacobianhandler.add_to_coefficient(end_equation_index, start_q_index, -1.0);
    jacobianhandler.add_to_coefficient(end_equation_index, end_q_index, 1.0);
  }

  void Controlvalve::d_evaluate_d_last_state(
      Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
      double /*new_time*/,
      const Eigen::Ref<const Eigen::VectorXd> & /*last_state*/,
      const Eigen::Ref<const Eigen::VectorXd> & /*new_state*/,
      const Eigen::Ref<const Eigen::VectorXd> & /*control*/) const {}

  void Controlvalve::d_evaluate_d_control(
      Aux::Matrixhandler &jacobianhandler, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const {
    auto start_equation_index = get_equation_start_index();
    auto pressure_control_index = get_control_startindex();

    jacobianhandler.add_to_coefficient(
        start_equation_index, pressure_control_index, 1.0);
  }

  void Controlvalve::set_initial_controls(
      Aux::InterpolatingVector_Base &full_control_vector,
      nlohmann::json const &control_json) const {
    set_simple_control_values(
        this, full_control_vector, control_json, get_control_schema());
  }

  void Controlvalve::set_lower_bounds(
      Aux::InterpolatingVector_Base &full_lower_bound_vector,
      nlohmann::json const &lower_bound_json) const {
    set_simple_control_values(
        this, full_lower_bound_vector, lower_bound_json, get_control_schema());
  }

  void Controlvalve::set_upper_bounds(
      Aux::InterpolatingVector_Base &full_upper_bound_vector,
      nlohmann::json const &upper_bound_json) const {
    set_simple_control_values(
        this, full_upper_bound_vector, upper_bound_json, get_control_schema());
  }

  Eigen::Index Controlvalve::needed_number_of_controls_per_time_point() const {
    return 1;
  }

  void Controlvalve::add_results_to_json(nlohmann::json &new_output) {
    std::string comp_type = Aux::component_class(*this);
    new_print_helper(new_output, comp_type, get_type());
  }

  std::string Controlvalve::componentclass() const {
    return Aux::component_class(*this);
  }
  std::string Controlvalve::componenttype() const { return get_type(); }
  std::string Controlvalve::id() const { return get_id(); }

} // namespace Model::Gas
