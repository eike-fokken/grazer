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
#include "SimpleControlcomponent.hpp"
#include "InterpolatingVector.hpp"
namespace Model {

  void set_simple_control_values(
      Controlcomponent const *controlcomponent,
      Aux::InterpolatingVector_Base &full_control_vector,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {

    Eigen::Ref<Eigen::VectorXd const> timepoints
        = full_control_vector.get_interpolation_points();

    auto initialvalues = Aux::InterpolatingVector::construct_from_json(
        initial_json, initial_schema);

    auto number_of_indices_per_step = initialvalues.get_inner_length();

    Eigen::Index index = 0;
    for (auto timepoint : timepoints) {
      full_control_vector.mut_timestep(index).segment(
          controlcomponent->get_control_startindex(),
          number_of_indices_per_step)
          = initialvalues(timepoint);
      ++index;
    }
  }

  Eigen::Index
  SimpleControlcomponent::set_control_indices(Eigen::Index next_free_index) {
    control_startindex = next_free_index;
    auto number_of_controls = needed_number_of_controls_per_time_point();
    control_afterindex = next_free_index + number_of_controls;

    return control_afterindex;
  }

} // namespace Model
