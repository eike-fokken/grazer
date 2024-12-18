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
#include "Constraintcomponent.hpp"
#include "Flowboundarynode.hpp"
#include "SimpleConstraintcomponent.hpp"

namespace Model::Gas {

  class ConstraintSink final :
      public Flowboundarynode,
      public SimpleConstraintcomponent {

  public:
    static nlohmann::json get_constraint_schema();
    static std::string get_type();

    ConstraintSink(nlohmann::json const &data);

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

    void set_constraint_lower_bounds(
        Aux::InterpolatingVector_Base &full_control_vector,
        nlohmann::json const &constraint_lower_bounds_json) const final;

    void set_constraint_upper_bounds(
        Aux::InterpolatingVector_Base &full_control_vector,
        nlohmann::json const &constraint_upper_bounds_json) const final;

    Eigen::Index needed_number_of_constraints_per_time_point() const final;
  };

} // namespace Model::Gas
