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
#pragma once
#include "Costcomponent.hpp"
#include "Edge.hpp"
#include "Gasedge.hpp"
#include "Shortcomponent.hpp"
#include "SimpleControlcomponent.hpp"
#include <nlohmann/json.hpp>

namespace Model::Gas {

  class Compressorstation final :
      public SimpleControlcomponent,
      public Costcomponent,
      public Shortcomponent {
  public:
    static std::string get_type();
    std::string get_gas_type() const final;

    Compressorstation(
        nlohmann::json const &edge_json,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    ///////////////////////////////////////////////////////////
    // Controlcomponent methods:
    ///////////////////////////////////////////////////////////

    static nlohmann::json get_control_schema();

    void setup() final;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, const Eigen::Ref<const Eigen::VectorXd> &last_state,
        const Eigen::Ref<const Eigen::VectorXd> &new_state,
        const Eigen::Ref<const Eigen::VectorXd> &control) const final;

    void d_evaluate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        const Eigen::Ref<const Eigen::VectorXd> &last_state,
        const Eigen::Ref<const Eigen::VectorXd> &new_state,
        const Eigen::Ref<const Eigen::VectorXd> &control) const final;

    void d_evaluate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        const Eigen::Ref<const Eigen::VectorXd> &last_state,
        const Eigen::Ref<const Eigen::VectorXd> &new_state,
        const Eigen::Ref<const Eigen::VectorXd> &control) const final;

    void d_evaluate_d_control(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void set_initial_controls(
        Aux::InterpolatingVector_Base &full_control_vector,
        nlohmann::json const &control_json) const final;

    void set_lower_bounds(
        Aux::InterpolatingVector_Base &full_lower_bound_vector,
        nlohmann::json const &lower_bound_json) const final;

    void set_upper_bounds(
        Aux::InterpolatingVector_Base &full_upper_bound_vector,
        nlohmann::json const &upper_bound_json) const final;

    Eigen::Index needed_number_of_controls_per_time_point() const final;

  private:
    std::string componentclass() const final;
    std::string componenttype() const final;
    std::string id() const final;

  public:
    ///////////////////////////////////////////////////////////
    // Statecomponent methods:
    ///////////////////////////////////////////////////////////

    void add_results_to_json(nlohmann::json &new_output) final;
    ///////////////////////////////////////////////////////////
    // Costcomponent methods:
    ///////////////////////////////////////////////////////////

    double evaluate_cost(
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void d_evaluate_cost_d_state(
        Aux::Matrixhandler &cost_new_state_jacobian_handler, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void d_evaluate_cost_d_control(
        Aux::Matrixhandler &cost_control_jacobian_handler, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    double evaluate_penalty(
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void d_evaluate_penalty_d_state(
        Aux::Matrixhandler &penalty_new_state_jacobian_handler, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void d_evaluate_penalty_d_control(
        Aux::Matrixhandler &penalty_control_jacobian_handler, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;
  };

} // namespace Model::Gas
