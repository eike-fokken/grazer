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
#include "SimpleStatecomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Statecomponent.hpp"
#include "schema_validation.hpp"
#include <iostream>

namespace Model {

  Eigen::VectorXd
  identity_converter(Eigen::Ref<Eigen::VectorXd const> const &x) {
    return x;
  }

  void set_simple_initial_values(
      Statecomponent const *component,
      Eigen::Ref<Eigen::VectorXd> vector_to_be_filled,
      nlohmann::json const &initial_json, nlohmann::json const &initial_schema,
      Eigen::Index number_of_points, double Delta_x,
      std::function<Eigen::VectorXd(Eigen::Ref<Eigen::VectorXd const> const &)>
          converter_function) {

    Aux::schema::validate_json(initial_json, initial_schema);

    // Create an InterpolatingVector it segment per segment.
    auto initialvalues = Aux::InterpolatingVector::construct_from_json(
        initial_json, initial_schema);
    auto number_of_indices_per_step = initialvalues.get_inner_length();

    if (number_of_indices_per_step * number_of_points
        > component->get_number_of_states()) {
      gthrow(
          {"You try to set more initial values than are reserved for "
           "component\n",
           ">>>", initial_json["id"], "<<<\n"});
    }
    auto current_index = component->get_state_startindex();
    for (Eigen::Index i = 0; i != number_of_points; ++i) {
      vector_to_be_filled.segment(current_index, number_of_indices_per_step)
          = converter_function(initialvalues(static_cast<double>(i) * Delta_x));
      current_index += number_of_indices_per_step;
    }
  }

  Eigen::Index
  SimpleStatecomponent::set_state_indices(Eigen::Index next_free_index) {
    state_startindex = next_free_index;
    auto number_of_states = needed_number_of_states();
    state_afterindex = next_free_index + number_of_states;

    return state_afterindex;
  }

} // namespace Model
