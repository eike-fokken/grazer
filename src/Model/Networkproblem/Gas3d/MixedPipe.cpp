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
#include "MixedPipe.hpp"
#include "Coloroutput.hpp"
#include "Edge.hpp"
#include "Exception.hpp"
#include "Get_base_component.hpp"
#include "Implicitboxscheme.hpp"
#include "Mathfunctions.hpp"
#include "Matrixhandler.hpp"
#include "Scheme_factory.hpp"
#include "Threepointscheme.hpp"
#include "TwoGasMixture.hpp"
#include "make_schema.hpp"
#include "unit_conversion.hpp"

#include <Eigen/Dense>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <type_traits>

namespace Model::Gas3d {

  std::string MixedPipe::get_type() { return "MixedPipe"; }
  std::string MixedPipe::get_gas_type() const { return get_type(); }

  int MixedPipe::init_vals_per_interpol_point() { return 3; }

  namespace unit = Aux::unit;

  nlohmann::json MixedPipe::get_schema() {
    nlohmann::json schema = Network::Edge::get_schema();

    Aux::schema::add_required(schema, "length", unit::length.get_schema());
    Aux::schema::add_required(schema, "diameter", unit::length.get_schema());
    Aux::schema::add_required(schema, "roughness", unit::length.get_schema());

    Aux::schema::add_required(
        schema, "desired_delta_x", Aux::schema::type::number());
    Aux::schema::add_required(
        schema, "balancelaw", Aux::schema::type::string());
    Aux::schema::add_required(schema, "scheme", Aux::schema::type::string());

    return schema;
  }

  nlohmann::json MixedPipe::get_initial_schema() {
    std::optional<int> interpol_points = std::nullopt;
    std::vector<nlohmann::json> contains_x
        = {R"({"maximum": 0, "minimum": 0})"_json}; // there is a point <= 0
    return Aux::schema::make_initial_schema(
        interpol_points, MixedPipe::init_vals_per_interpol_point(), contains_x);
  }

  MixedPipe::MixedPipe(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes) :
      Network::Edge(topology, nodes),
      number_of_points(
          static_cast<int>(std::ceil(
              unit::length.parse_to_si(topology["length"])
              / topology["desired_delta_x"].get<double>()))
          + 1),
      Delta_x(
          unit::length.parse_to_si(topology["length"])
          / (number_of_points - 1)),
      mixed_gas_law(topology),
      scheme{Scheme::make_threepointscheme<3>(topology)} {}

  MixedPipe::~MixedPipe() {}

  void MixedPipe::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    for (auto i = get_equation_start_index(); i != get_equation_after_index();
         i += 3) {
      scheme->evaluate_point(
          i, Delta_x, mixed_gas_law, rootvalues, last_time, new_time,
          last_state, new_state);
    }
  }

  void MixedPipe::d_evaluate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    for (auto i = get_equation_start_index(); i != get_equation_after_index();
         i += 3) {
      scheme->d_evaluate_point_d_new_state(
          i, Delta_x, mixed_gas_law, jacobianhandler, last_time, new_time,
          last_state, new_state);
    }
  }
  void MixedPipe::d_evaluate_d_last_state(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    for (auto i = get_equation_start_index(); i != get_equation_after_index();
         i += 3) {
      scheme->d_evaluate_point_d_last_state(
          i, Delta_x, mixed_gas_law, jacobianhandler, last_time, new_time,
          last_state, new_state);
    }
  }

  void MixedPipe::setup() { setup_output_json_helper(get_id()); }

  Eigen::Index MixedPipe::needed_number_of_states() const {
    return 3 * number_of_points;
  }

  void MixedPipe::add_results_to_json(nlohmann::json &new_output) {
    auto &this_output_json = get_output_json_ref();
    std::string comp_type = Aux::component_class(*this);
    new_output[comp_type][get_type()].push_back(std::move(this_output_json));
  }

  void MixedPipe::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {

    nlohmann::json current_value;
    current_value["time"] = time;
    for (int i = 0; i != number_of_points; ++i) {
      Eigen::Vector3d current_state
          = state.segment<3>(get_state_startindex() + 3 * i);
      double current_rho1 = current_state[0];
      double current_rho2 = current_state[1];
      double current_q = current_state[2];
      double x = i * Delta_x;
      nlohmann::json rho1_json;
      rho1_json["x"] = x;
      rho1_json["value"] = current_rho1;
      current_value["rho1"].push_back(rho1_json);

      nlohmann::json rho2_json;
      rho2_json["x"] = x;
      rho2_json["value"] = current_rho2;
      current_value["rho2"].push_back(rho2_json);

      nlohmann::json flow_json;
      flow_json["x"] = x;
      flow_json["value"] = current_q;
      current_value["flow"].push_back(flow_json);
    }
    auto &output_json = get_output_json_ref();
    output_json["data"].push_back(std::move(current_value));
  }

  void MixedPipe::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) const {

    set_simple_initial_values(
        this, new_state, initial_json, get_initial_schema(), number_of_points,
        Delta_x);
  }

  Balancelaw::TwoGasMixture const &MixedPipe::get_balancelaw() const {
    return mixed_gas_law;
  }

  int MixedPipe::get_number_of_points() const { return number_of_points; }
  double MixedPipe::get_Delta_x() const { return Delta_x; }

  double MixedPipe::get_length() const {
    return (number_of_points - 1) * Delta_x;
  }

} // namespace Model::Gas3d
