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
#include "ConstraintSink.hpp"
#include "Matrixhandler.hpp"
#include "SimpleConstraintcomponent.hpp"
#include "make_schema.hpp"

namespace Model::Gas {

  nlohmann::json ConstraintSink::get_constraint_schema() {
    return Aux::schema::make_boundary_schema(1);
  }
  std::string ConstraintSink::get_type() { return "ConstraintSink"; }

  ConstraintSink::ConstraintSink(nlohmann::json const &data) :
      Flowboundarynode(revert_boundary_conditions(data)) {}

  void ConstraintSink::evaluate_constraint(
      Eigen::Ref<Eigen::VectorXd> constraint_values, double /*time*/,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const {
    auto first_direction = directed_attached_gas_edges[0].first;
    auto sample_gas_edge = directed_attached_gas_edges[0].second;
    auto sample_boundary_state
        = sample_gas_edge->get_boundary_state(first_direction, state);
    auto sample_pressure = sample_boundary_state[0];

    constraint_values[get_constraint_startindex()] = sample_pressure;
  }

  void ConstraintSink::d_evaluate_constraint_d_state(
      Aux::Matrixhandler &constraint_new_state_jacobian_handler, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &) const {
    auto first_direction = directed_attached_gas_edges[0].first;
    auto *sample_gas_edge = directed_attached_gas_edges[0].second;
    auto index = sample_gas_edge->get_boundary_state_index(first_direction);
    constraint_new_state_jacobian_handler.add_to_coefficient(
        get_constraint_startindex(), index, 1.0);
  }

  void ConstraintSink::d_evaluate_constraint_d_control(
      Aux::Matrixhandler &, double, Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &) const {}

  void ConstraintSink::set_constraint_lower_bounds(
      Aux::InterpolatingVector_Base &full_lower_bound_vector,
      const nlohmann::json &constraint_lower_bounds_json) const {
    set_simple_constraint_values(
        this, full_lower_bound_vector, constraint_lower_bounds_json,
        get_constraint_schema());
  }

  void ConstraintSink::set_constraint_upper_bounds(
      Aux::InterpolatingVector_Base &full_upper_bound_vector,
      const nlohmann::json &constraint_upper_bounds_json) const {
    set_simple_constraint_values(
        this, full_upper_bound_vector, constraint_upper_bounds_json,
        get_constraint_schema());
  }

  Eigen::Index
  ConstraintSink::needed_number_of_constraints_per_time_point() const {
    return 1;
  }
} // namespace Model::Gas
