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
#include "Eigen/Sparse"
#include "Timedata.hpp"
#include <nlohmann/json.hpp>

namespace Aux {
  class InterpolatingVector_Base;
  class Matrixhandler;
} // namespace Aux
class Mock_OptimizableObject;
namespace Model {
  class SimpleConstraintcomponent;

  class Constraintcomponent {
    /** \brief SimpleConstraintcomponent is a friend of Constraintcomponent to
     * give it access to #start_constraint_index and #after_constraint_index.
     */
    friend class SimpleConstraintcomponent;
    friend class Networkproblem;
    friend class ::Mock_OptimizableObject;

  public:
    static nlohmann::json get_constraint_schema() = delete;

    virtual ~Constraintcomponent(){};

    virtual void evaluate_constraint(
        Eigen::Ref<Eigen::VectorXd> constraint_values, double time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const
        = 0;

    virtual void d_evaluate_constraint_d_state(
        Aux::Matrixhandler &constraint_new_state_jacobian_handler, double time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const
        = 0;

    virtual void d_evaluate_constraint_d_control(
        Aux::Matrixhandler &constraint_control_jacobian_handler, double time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const
        = 0;

    /** \brief This function sets the indices #start_constraint_index and
     * #after_constraint_index.
     *
     * @param next_free_index The first constraint index that is currently
     * not claimed by another component.
     * @returns The new lowest free index.
     */
    virtual Eigen::Index set_constraint_indices(Eigen::Index next_free_index)
        = 0;

    Eigen::Index get_number_of_constraints_per_timepoint() const;

    /** \brief getter for #start_constraint_index
     */
    Eigen::Index get_constraint_startindex() const;

    /** \brief getter for #after_constraint_index
     */
    Eigen::Index get_constraint_afterindex() const;

    virtual void set_constraint_lower_bounds(
        Aux::InterpolatingVector_Base &full_control_vector,
        nlohmann::json const &constraint_lower_bounds_json) const
        = 0;

    virtual void set_constraint_upper_bounds(
        Aux::InterpolatingVector_Base &full_control_vector,
        nlohmann::json const &constraint_upper_bounds_json) const
        = 0;

  private:
    /** \brief The first index, this Constraintcomponent "owns".
     */
    Eigen::Index constraint_startindex{-1};

    /** \brief The first index after #start_constraint_index, that is not
     * "owned" by this Constraintcomponent.
     */
    Eigen::Index constraint_afterindex{-1};
  };
} // namespace Model
