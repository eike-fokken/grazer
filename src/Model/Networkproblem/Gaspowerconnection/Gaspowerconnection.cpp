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
#include "Gaspowerconnection.hpp"
#include "Exception.hpp"
#include "ExternalPowerplant.hpp"
#include "Get_base_component.hpp"
#include "Mathfunctions.hpp"
#include "Matrixhandler.hpp"
#include "Misc.hpp"
#include "Node.hpp"
#include "Powernode.hpp"
#include "make_schema.hpp"
#include <fstream>
#include <iostream>

namespace Model::Gaspowerconnection {

  std::string Gaspowerconnection::get_type() { return "Gaspowerconnection"; }
  std::string Gaspowerconnection::get_gas_type() const { return get_type(); }
  nlohmann::json Gaspowerconnection::get_schema() {
    nlohmann::json schema = Network::Edge::get_schema();

    Aux::schema::add_required(
        schema, "gas2power_q_coeff", Aux::schema::type::number());
    Aux::schema::add_required(
        schema, "power2gas_q_coeff", Aux::schema::type::number());

    return schema;
  }

  nlohmann::json Gaspowerconnection::get_initial_schema() {
    std::vector<nlohmann::json> contains_x
        = {R"({"minimum": 0, "maximum": 0})"_json};
    int interpol_points = 1;
    return Aux::schema::make_initial_schema(
        interpol_points, Gaspowerconnection::init_vals_per_interpol_point(),
        contains_x);
  }

  Gaspowerconnection::Gaspowerconnection(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes) :
      Network::Edge(topology, nodes),
      gas2power_q_coefficient(topology["gas2power_q_coeff"].get<double>()),
      power2gas_q_coefficient(topology["power2gas_q_coeff"].get<double>()) {}

  void Gaspowerconnection::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double /*last_time*/,
      double /*new_time*/, Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto q_index = get_state_startindex() + 1;
    rootvalues[q_index]
        = powerendnode->P(new_state) - generated_power(new_state[q_index]);
  }

  void Gaspowerconnection::d_evaluate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto p_index = get_state_startindex();
    auto q_index = get_state_startindex() + 1;
    auto q = new_state[q_index];
    jacobianhandler.add_to_coefficient(
        q_index, q_index, -dgenerated_power_dq(q));
    powerendnode->evaluate_P_derivative(q_index, jacobianhandler, new_state);
    jacobianhandler.add_to_coefficient(
        powerendnode->get_state_startindex(), p_index, 0.0);
  }

  void Gaspowerconnection::d_evaluate_d_last_state(
      Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const {}

  void Gaspowerconnection::setup() {

    setup_output_json_helper(get_id());

    if (powerendnode != nullptr) {
      std::cout << "You are calling setup a second time!" << std::endl;
    }
    auto powernodeptr
        = dynamic_cast<Model::Gaspowerconnection::ExternalPowerplant *>(
            get_ending_node());
    if (powernodeptr == nullptr) {
      gthrow(
          {"An edge of type \"Gaspowerconnection\" can only end at an "
           "\"ExternalPowerplant\" but its ending "
           "node, ",
           get_ending_node()->get_id(), " is not an ExternalPowerplant."});
    }
    powerendnode = powernodeptr;
  }

  Eigen::Index Gaspowerconnection::needed_number_of_states() const { return 2; }

  void Gaspowerconnection::add_results_to_json(nlohmann::json &new_output) {
    auto &this_output_json = get_output_json_ref();
    std::string comp_type = Aux::component_class(*this);
    new_output[comp_type][get_type()].push_back(std::move(this_output_json));
  }

  void Gaspowerconnection::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {

    nlohmann::json current_value;
    current_value["time"] = time;
    current_value["pressure"] = nlohmann::json::array();
    auto start_state = get_boundary_state(Gas::start, state);
    nlohmann::json pressure0_json;
    pressure0_json["x"] = 0.0;
    pressure0_json["value"] = start_state[0];
    current_value["pressure"].push_back(pressure0_json);

    nlohmann::json flow0_json;
    flow0_json["x"] = 0.0;
    flow0_json["value"] = start_state[1];
    current_value["flow"].push_back(flow0_json);

    auto &output_json = get_output_json_ref();
    output_json["data"].push_back(std::move(current_value));
  }

  void Gaspowerconnection::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) const {
    set_simple_initial_values(
        this, new_state, initial_json, get_initial_schema());
  }

  Eigen::Vector2d Gaspowerconnection::get_boundary_p_qvol_bar(
      Gas::Direction direction,
      Eigen::Ref<Eigen::VectorXd const> const &state) const {
    return get_boundary_state(direction, state);
  }

  void Gaspowerconnection::dboundary_p_qvol_dstate(
      Gas::Direction direction, Aux::Matrixhandler &jacobianhandler,
      Eigen::RowVector2d function_derivative, Eigen::Index rootvalues_index,
      Eigen::Ref<Eigen::VectorXd const> const &) const {
    auto p_index = get_boundary_state_index(direction);
    auto q_index = p_index + 1;
    jacobianhandler.add_to_coefficient(
        rootvalues_index, p_index, function_derivative[0]);
    jacobianhandler.add_to_coefficient(
        rootvalues_index, q_index, function_derivative[1]);
  }

  double Gaspowerconnection::smoothing_polynomial(double q) const {
    if (q > kappa + Aux::EPSILON or q < -kappa - Aux::EPSILON) {
      gthrow(
          {" You can't call this function for values of q bigger than ",
           Aux::to_string_precise(kappa, 12), "\n"});
    }
    double rel = q / kappa;
    return q
           * (0.5 * (gas2power_q_coefficient + power2gas_q_coefficient)
              - 0.75 * (power2gas_q_coefficient - gas2power_q_coefficient) * rel
              + 0.25 * (power2gas_q_coefficient - gas2power_q_coefficient) * rel
                    * rel * rel);
  }
  double Gaspowerconnection::dsmoothing_polynomial_dq(double q) const {
    if (q > kappa + Aux::EPSILON or q < -kappa - Aux::EPSILON) {
      gthrow(
          {" You can't call this function for values of q bigger than ",
           Aux::to_string_precise(kappa, 12), "\n"});
    }
    double rel = q / kappa;
    return 0.5 * (gas2power_q_coefficient + power2gas_q_coefficient)
           - 1.5 * (power2gas_q_coefficient - gas2power_q_coefficient) * rel
           + (power2gas_q_coefficient - gas2power_q_coefficient) * rel * rel
                 * rel;
  }

  double Gaspowerconnection::generated_power(double q) const {
    if (q > kappa) {
      return gas2power_q_coefficient * q;
    } else if (q < -kappa) {
      return power2gas_q_coefficient * q;
    } else {
      return smoothing_polynomial(q);
    }
  }
  double Gaspowerconnection::dgenerated_power_dq(double q) const {
    if (q > kappa) {
      return gas2power_q_coefficient;
    } else if (q < -kappa) {
      return power2gas_q_coefficient;
    } else {
      return dsmoothing_polynomial_dq(q);
    }
  }

} // namespace Model::Gaspowerconnection
