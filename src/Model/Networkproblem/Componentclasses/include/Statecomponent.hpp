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
#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
// #include "Equationcomponent.hpp"
class Mock_OptimizableObject;
namespace Model {

  class Statecomponent {
    /** \brief SimpleStatecomponent is a friend of Statecomponent to give it
     * access to #start_state_index and #after_state_index.
     */
    friend class SimpleStatecomponent;
    friend class Networkproblem;
    friend class ::Mock_OptimizableObject;

  public:
    static nlohmann::json get_initial_schema() = delete;

    /// \brief This function sets the indices #start_state_index and
    /// #after_state_index.
    ///
    /// @param next_free_index The first state index that is currently not
    /// claimed by another component.
    /// @returns The new lowest free index.
    virtual Eigen::Index set_state_indices(Eigen::Index next_free_index) = 0;

    Eigen::Index get_number_of_states() const;

    /// \brief getter for #start_state_index
    Eigen::Index get_state_startindex() const;

    /// \brief getter for #after_state_index
    Eigen::Index get_state_afterindex() const;

    virtual void add_results_to_json(nlohmann::json &) = 0;

    /// \brief Stores computed values in an output json.
    virtual void
    json_save(double time, Eigen::Ref<Eigen::VectorXd const> const &state)
        = 0;

    /// \brief Fills the indices owned by this component with initial values
    /// from a json.
    ///
    /// Relies on the exact format of the json. \todo { Document the intial json
    /// format. }
    /// @param[out] new_state state vector, who shall contain the initial
    /// values.
    /// @param initial_json Json object that contains the initial values.
    virtual void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) const = 0;

  protected:
    void setup_output_json_helper(std::string const &id);

    nlohmann::json &get_output_json_ref();

  private:
    /// \brief holds the computed values of this component.
    ///
    /// Contains exactly two keys: "time" and "data"
    /// Time holds a number representing the time in seconds.
    /// Data holds an array of jsons, whose layout depends on the component in
    /// question.
    nlohmann::json component_output;

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
  };
} // namespace Model
