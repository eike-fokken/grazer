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
#include "SimpleSwitchcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"

namespace Model {

  void set_simple_switch_values(
      Switchcomponent const *switchcomponent,
      Aux::InterpolatingVector_Base &full_switch_vector,
      nlohmann::json const &initial_json,
      nlohmann::json const &initial_schema) {

    Eigen::Ref<Eigen::VectorXd const> timepoints
        = full_switch_vector.get_interpolation_points();

    auto initialvalues = Aux::InterpolatingVector::construct_from_json(
        initial_json, initial_schema);

    auto number_of_indices_per_step = initialvalues.get_inner_length();

    Eigen::Index index = 0;
    for (auto timepoint : timepoints) {
      full_switch_vector.mut_timestep(index).segment(
          switchcomponent->get_switch_startindex(), number_of_indices_per_step)
          = initialvalues(timepoint);
      ++index;
    }
  }

  Eigen::Index
  SimpleSwitchcomponent::set_switch_indices(Eigen::Index next_free_index) {
    switch_startindex = next_free_index;
    auto number_of_switches = needed_number_of_switches_per_time_point();
    switch_afterindex = next_free_index + number_of_switches;

    return switch_afterindex;
  }

  Eigen::Index SimpleSwitchcomponent::get_switch_startindex() const {
    if (switch_startindex < 0) {
      gthrow(
          {"switch_startindex < 0. Probably ", __func__, " was called ",
           "before calling set_indices().\n This is forbidden."});
    }
    return switch_startindex;
  }
  Eigen::Index SimpleSwitchcomponent::get_switch_afterindex() const {
    if (switch_afterindex < 0) {
      gthrow(
          {"switch_afterindex < 0. Probably ", __func__,
           " was called "
           "before calling set_indices().\n This is forbidden."});
    }
    return switch_afterindex;
  }

} // namespace Model
