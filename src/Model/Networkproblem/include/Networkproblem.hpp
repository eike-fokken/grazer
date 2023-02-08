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
#include "OptimizableObject.hpp"
#include "Statecomponent.hpp"
#include "Timedata.hpp"
#include <Eigen/Sparse>
#include <memory>
#include <vector>

namespace Network {
  class Net;
}

// This namespace holds all data relating to problems that construct the model
// equations from a network from the namespace Network.
namespace Model {

  class Equationcomponent;

  // This class defines a problem, that builds the model equations from a
  // network.
  class Networkproblem final : public OptimizableObject {

  public:
    static std::string get_type();

    ~Networkproblem();

    Networkproblem(std::unique_ptr<Network::Net> _network);

    /** \brief Initializes a Networkproblem to a valid state.
     *
     * init() calls all of #set_state_indices(), #set_control_indices() and
     * #set_constraint_indices() on the appropriate components in
     * #statecomponents, #controlcomponents and #constraintcomponents. Also, it
     * calls #setup() on all elements in equationcomponents and
     * controlcomponents.
     *
     * @param next_free_state_index The greatest unclaimed index auf the state
     * vector (before calling this function), defaulted to 0.
     * @param next_free_control_index The greatest unclaimed index auf the
     * control vector (before calling this function), defaulted to 0.
     * @param next_free_constraint_index The greatest unclaimed index auf the
     * constraint vector (before calling this function), defaulted to 0.
     */
    void init(
        Eigen::Index next_free_state_index = 0,
        Eigen::Index next_free_control_index = 0,
        Eigen::Index next_free_constraint_index = 0);

    ////////////////////////////////////////////////////////////////////////////
    // Statecomponent methods
    ////////////////////////////////////////////////////////////////////////////

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) final;

    void add_results_to_json(nlohmann::json &new_output) final;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) const final;

    Eigen::Index set_state_indices(Eigen::Index next_free_index) final;

    Eigen::Index get_state_startindex() const final;

    Eigen::Index get_state_afterindex() const final;

  private:
    /// \brief The first index, this Equationcomponent "owns".
    ///
    /// Most equation components write only to their own indices between
    /// #start_state_index (inclusive) and #after_state_index (exclusive).
    /// There are exceptions though, e.g. instances of \ref
    /// Model::Gas::Gasnode "Gasnode".
    Eigen::Index state_startindex{-1};

    /// \brief The first index after #start_state_index, that is not "owned" by
    /// this Equationcomponent.
    ///
    /// Most equation components write only to their own indices between
    /// #start_state_index (inclusive) and #after_state_index (exclusive).
    /// There are exceptions though, e.g. instances of
    /// \ref Model::Gas::Gasnode "Gasnode".
    Eigen::Index state_afterindex{-1};

    ////////////////////////////////////////////////////////////////////////////
    // Equationcomponent methods
    ////////////////////////////////////////////////////////////////////////////
  public:
    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) final;

    void d_evaluate_d_new_state(
        Aux::Matrixhandler &jacobian, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void d_evaluate_d_last_state(
        Aux::Matrixhandler &jacobian, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void setup() final;

    ////////////////////////////////////////////////////////////////////////////
    // Controlcomponent methods
    ////////////////////////////////////////////////////////////////////////////

    void d_evaluate_d_control(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void set_initial_controls(
        Aux::InterpolatingVector_Base &full_control_vector,
        nlohmann::json const &control_json) const final;

    /** \brief getter for #start_control_index
     */
    Eigen::Index get_control_startindex() const final;

    /** \brief getter for #after_control_index
     */
    Eigen::Index get_control_afterindex() const final;

    void set_lower_bounds(
        Aux::InterpolatingVector_Base &lower_bounds_vector,
        nlohmann::json const &lower_bounds_json) const final;

    void set_upper_bounds(
        Aux::InterpolatingVector_Base &upper_bounds_vector,
        nlohmann::json const &upper_bounds_json) const final;

    Eigen::Index set_control_indices(Eigen::Index next_free_index) final;

    void save_controls_to_json(
        Aux::InterpolatingVector_Base const &controls,
        nlohmann::json &json) final;

  private:
    /** \brief The first control index, this Controlcomponent "owns".
     */
    Eigen::Index control_startindex{-1};

    /** \brief The first control index after #start_control_index, that is
     * not "owned" by this Controlcomponent.
     */
    Eigen::Index control_afterindex{-1};

    std::string componentclass() const final;
    std::string componenttype() const final;
    std::string id() const final;

    ////////////////////////////////////////////////////////////////////////////
    // Switchcomponent methods
    ////////////////////////////////////////////////////////////////////////////

    void set_initial_switches(
        Aux::InterpolatingVector_Base &full_switch_vector,
        nlohmann::json const &switch_json) const final;

    /** \brief getter for #start_switch_index
     */
    Eigen::Index get_switch_startindex() const final;

    /** \brief getter for #after_switch_index
     */
    Eigen::Index get_switch_afterindex() const final;

    Eigen::Index set_switch_indices(Eigen::Index next_free_index) final;

    void save_switches_to_json(
        Aux::InterpolatingVector_Base const &switches,
        nlohmann::json &json) final;

  private:
    /** \brief The first switch index, this Switchcomponent "owns".
     */
    Eigen::Index switch_startindex{-1};

    /** \brief The first switch index after #start_switch_index, that is
     * not "owned" by this Switchcomponent.
     */
    Eigen::Index switch_afterindex{-1};

    /////////////////////////////////////////////////////////
    // cost function methods:
    /////////////////////////////////////////////////////////
  public:
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

    /////////////////////////////////////////////////////////
    // constraint methods:
    /////////////////////////////////////////////////////////

    void evaluate_constraint(
        Eigen::Ref<Eigen::VectorXd> constraint_values, double time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void d_evaluate_constraint_d_state(
        Aux::Matrixhandler &constraint_new_state_jacobian_handler, double time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void d_evaluate_constraint_d_control(
        Aux::Matrixhandler &constraint_control_jacobian_handler, double time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    Eigen::Index set_constraint_indices(Eigen::Index next_free_index) final;

    void set_constraint_lower_bounds(
        Aux::InterpolatingVector_Base &full_control_vector,
        nlohmann::json const &constraint_lower_bounds_json) const final;

    void set_constraint_upper_bounds(
        Aux::InterpolatingVector_Base &full_control_vector,
        nlohmann::json const &constraint_upper_bounds_json) const final;

    // ////////////////////////////////////////////////////////////////////////////
    // // Switchcomponent methods
    // ////////////////////////////////////////////////////////////////////////////

    // Eigen::Index set_switch_indices(Eigen::Index next_free_index) final;

    // void set_initial_switches(
    //     Aux::InterpolatingVector_Base &full_switch_vector,
    //     nlohmann::json const &switch_json) const final;

    // void save_switches_to_json(
    //     Aux::InterpolatingVector_Base const &switches,
    //     nlohmann::json &json) final;

    /////////////////////////////////////////////////////////
    // other methods:
    /////////////////////////////////////////////////////////
  public:
    Network::Net const &get_network() const;

  private:
    std::unique_ptr<Network::Net> network;
    std::vector<Equationcomponent *> equationcomponents;
    std::vector<Statecomponent *> statecomponents;
    std::vector<Controlcomponent *> controlcomponents;
    std::vector<Switchcomponent *> switchcomponents;
    std::vector<Costcomponent *> costcomponents;
    std::vector<Constraintcomponent *> constraintcomponents;
  };

} // namespace Model
