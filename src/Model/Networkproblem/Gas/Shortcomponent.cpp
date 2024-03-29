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
#include "Shortcomponent.hpp"
#include "Exception.hpp"
#include "Matrixhandler.hpp"
#include "make_schema.hpp"
#include <fstream>
#include <iostream>

namespace Model::Gas {

  nlohmann::json Shortcomponent::get_initial_schema() {
    int interpol_points = 2;
    std::vector<nlohmann::json> contains_x
        = {R"({"minimum": 0, "maximum": 0})"_json,
           R"({"minimum": 1, "maximum": 1})"_json};
    return Aux::schema::make_initial_schema(
        interpol_points, Gasedge::init_vals_per_interpol_point(), contains_x);
  }
  void Shortcomponent::new_print_helper(
      nlohmann::json &new_output, std::string const &component_type,
      std::string const &type) {
    auto &this_output_json = get_output_json_ref();
    new_output[component_type][type].push_back(std::move(this_output_json));
  }

  Eigen::Index Shortcomponent::needed_number_of_states() const {
    return number_of_state_variables;
  }

  void Shortcomponent::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {

    nlohmann::json current_value;
    current_value["time"] = time;
    current_value["pressure"] = nlohmann::json::array();
    auto start_state = get_boundary_state(start, state);
    auto end_state = get_boundary_state(end, state);
    nlohmann::json pressure0_json;
    pressure0_json["x"] = 0.0;
    pressure0_json["value"] = start_state[0];
    current_value["pressure"].push_back(pressure0_json);

    nlohmann::json pressure1_json;
    pressure1_json["x"] = 1.0;
    pressure1_json["value"] = end_state[0];
    current_value["pressure"].push_back(pressure1_json);
    nlohmann::json flow0_json;
    flow0_json["x"] = 0.0;
    flow0_json["value"] = start_state[1];
    current_value["flow"].push_back(flow0_json);

    nlohmann::json flow1_json;
    flow1_json["x"] = 1.0;
    flow1_json["value"] = end_state[1];
    current_value["flow"].push_back(flow1_json);

    auto &output_json = get_output_json_ref();
    output_json["data"].push_back(std::move(current_value));
  }

  void Shortcomponent::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      const nlohmann::json &initial_json) const {
    set_simple_initial_values(
        this, new_state, initial_json, get_initial_schema(), 2);
  }

  Eigen::Vector2d Shortcomponent::get_boundary_p_qvol_bar(
      Direction direction,
      Eigen::Ref<Eigen::VectorXd const> const &state) const {
    return get_boundary_state(direction, state);
  }

  void Shortcomponent::dboundary_p_qvol_dstate(
      Direction direction, Aux::Matrixhandler &jacobianhandler,
      Eigen::RowVector2d function_derivative, Eigen::Index rootvalues_index,
      Eigen::Ref<Eigen::VectorXd const> const &) const {
    auto p_index = get_boundary_state_index(direction);
    auto q_index = p_index + 1;

    jacobianhandler.add_to_coefficient(
        rootvalues_index, p_index, function_derivative[0]);
    jacobianhandler.add_to_coefficient(
        rootvalues_index, q_index, function_derivative[1]);
  }

} // namespace Model::Gas
