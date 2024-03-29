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
#include "Powernode.hpp"
#include "Exception.hpp"
#include "Get_base_component.hpp"
#include "InterpolatingVector.hpp"
#include "Matrixhandler.hpp"
#include "SimpleStatecomponent.hpp"
#include "Statecomponent.hpp"
#include "Transmissionline.hpp"
#include "make_schema.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <tuple>
#include <utility>

namespace Model::Power {

  nlohmann::json Powernode::get_schema() {
    nlohmann::json schema = Network::Node::get_schema();
    Aux::schema::add_required(schema, "B", Aux::schema::type::number());
    Aux::schema::add_required(schema, "G", Aux::schema::type::number());

    return schema;
  }

  nlohmann::json Powernode::get_boundary_schema() {
    return Aux::schema::make_boundary_schema(2);
  }

  nlohmann::json Powernode::get_initial_schema() {
    int interpol_points = 1;
    std::vector<nlohmann::json> contains_x
        = {R"({"minimum": 0, "maximum": 0})"_json};
    return Aux::schema::make_initial_schema(
        interpol_points, number_of_state_variables, contains_x);
  }

  Powernode::Powernode(nlohmann::json const &topology) :
      Node(topology),
      boundaryvalue(Aux::InterpolatingVector::construct_from_json(
          topology["boundary_values"], get_boundary_schema())),
      G(topology["G"]),
      B(topology["B"]) {}

  Eigen::Index Powernode::needed_number_of_states() const {
    return number_of_state_variables;
  }

  void Powernode::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) const {
    set_simple_initial_values(
        this, new_state, initial_json, get_initial_schema());
  }

  void Powernode::setup_helper() {
    setup_output_json_helper(get_id());
    attached_component_data.clear();

    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      if (!line) {
        continue;
      }
      auto *endnode = line->get_ending_powernode();

      auto line_G = line->get_G();
      auto line_B = line->get_B();

      attached_component_data.push_back({line_G, line_B, endnode});
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      if (!line) {
        continue;
      }
      auto *startnode = line->get_starting_powernode();

      auto line_G = line->get_G();
      auto line_B = line->get_B();

      attached_component_data.push_back({line_G, line_B, startnode});
    }
  }

  double Powernode::get_G() const { return G; }

  double Powernode::get_B() const { return B; }

  void Powernode::add_results_to_json(nlohmann::json &new_output) {
    auto &this_output_json = get_output_json_ref();
    std::string comp_type = Aux::component_class(*this);
    new_output[comp_type][get_power_type()].push_back(
        std::move(this_output_json));
  }

  void Powernode::json_save_power(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state, double P_val,
      double Q_val) {
    nlohmann::json current_value;
    current_value["time"] = time;
    current_value["P"] = P_val;
    current_value["Q"] = Q_val;
    current_value["V"] = state[get_state_startindex()];
    current_value["phi"] = state[get_state_startindex() + 1];

    auto &output_json = get_output_json_ref();
    output_json["data"].push_back(std::move(current_value));
  }

  double Powernode::P(Eigen::Ref<Eigen::VectorXd const> const &state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    auto G_i = get_G();

    auto V_i = state[V_index];
    auto phi_i = state[phi_index];

    double P = 0.0;
    P += G_i * V_i * V_i;
    for (auto &triple : attached_component_data) {
      auto G_ik = std::get<0>(triple);
      auto B_ik = std::get<1>(triple);
      auto *othernode = std::get<2>(triple);
      auto V_index_k = othernode->get_state_startindex();
      auto phi_index_k = V_index_k + 1;
      auto V_k = state[V_index_k];
      auto phi_k = state[phi_index_k];
      auto phi_ik = phi_i - phi_k;
      P += V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik));
    }
    return P;
  }

  double Powernode::Q(Eigen::Ref<Eigen::VectorXd const> const &state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    auto B_i = get_B();
    auto V_i = state[V_index];
    auto phi_i = state[phi_index];

    double Q = 0.0;
    Q -= B_i * V_i * V_i;
    for (auto &triple : attached_component_data) {

      auto G_ik = std::get<0>(triple);
      auto B_ik = std::get<1>(triple);
      auto *othernode = std::get<2>(triple);
      auto V_index_k = othernode->get_state_startindex();
      auto phi_index_k = V_index_k + 1;
      auto V_k = state[V_index_k];
      auto phi_k = state[phi_index_k];
      auto phi_ik = phi_i - phi_k;
      Q += V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik));
    }
    return Q;
  }

  void Powernode::evaluate_P_derivative(
      Eigen::Index equationindex, Aux::Matrixhandler &jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    auto G_i = get_G();
    auto V_i = new_state[V_index];
    auto phi_i = new_state[phi_index];

    jacobianhandler.add_to_coefficient(equationindex, V_index, 2 * G_i * V_i);
    jacobianhandler.add_to_coefficient(equationindex, phi_index, 0.0);

    for (auto &triple : attached_component_data) {
      auto G_ik = std::get<0>(triple);
      auto B_ik = std::get<1>(triple);
      auto *othernode = std::get<2>(triple);
      auto V_index_k = othernode->get_state_startindex();
      auto phi_index_k = V_index_k + 1;
      auto V_k = new_state[V_index_k];
      auto phi_k = new_state[phi_index_k];
      auto phi_ik = phi_i - phi_k;

      jacobianhandler.add_to_coefficient(
          equationindex, V_index,
          V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler.add_to_coefficient(
          equationindex, phi_index,
          V_i * V_k * (-G_ik * sin(phi_ik) + B_ik * cos(phi_ik)));

      jacobianhandler.add_to_coefficient(
          equationindex, V_index_k,
          V_i * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler.add_to_coefficient(
          equationindex, phi_index_k,
          V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
    }
  }

  void Powernode::evaluate_Q_derivative(
      Eigen::Index equationindex, Aux::Matrixhandler &jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    auto B_i = get_B();
    auto V_i = new_state[V_index];
    auto phi_i = new_state[phi_index];

    jacobianhandler.add_to_coefficient(equationindex, V_index, -2 * B_i * V_i);
    jacobianhandler.add_to_coefficient(equationindex, phi_index, 0.0);

    for (auto &triple : attached_component_data) {
      auto G_ik = std::get<0>(triple);
      auto B_ik = std::get<1>(triple);
      auto *othernode = std::get<2>(triple);
      auto V_index_k = othernode->get_state_startindex();
      auto phi_index_k = V_index_k + 1;
      auto V_k = new_state[V_index_k];
      auto phi_k = new_state[phi_index_k];
      auto phi_ik = phi_i - phi_k;

      jacobianhandler.add_to_coefficient(
          equationindex, V_index,
          V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler.add_to_coefficient(
          equationindex, phi_index,
          V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler.add_to_coefficient(
          equationindex, V_index_k,
          V_i * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler.add_to_coefficient(
          equationindex, phi_index_k,
          V_i * V_k * (-G_ik * cos(phi_ik) - B_ik * sin(phi_ik)));
    }
  }

} // namespace Model::Power
