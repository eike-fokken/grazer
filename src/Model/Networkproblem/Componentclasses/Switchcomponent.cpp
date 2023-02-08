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
#include "Switchcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include <nlohmann/json.hpp>

namespace Model {

  Eigen::Index Switchcomponent::get_number_of_switches_per_timepoint() const {
    return get_switch_afterindex() - get_switch_startindex();
  }

  void Switchcomponent::save_switches_to_json(
      Aux::InterpolatingVector_Base const &switches, nlohmann::json &json) {
    auto this_class = componentclass();
    auto this_type = componenttype();
    if (not json.contains(this_class)) {
      json[this_class] = nlohmann::json::object();
    }
    if (not json[this_class].contains(this_type)) {
      json[this_class][this_type] = nlohmann::json::array();
    }

    nlohmann::json output_json;
    output_json["id"] = id();
    output_json["data"] = nlohmann::json::array();

    for (Eigen::Index index = 0; index != switches.size(); ++index) {
      nlohmann::json this_timestep;
      this_timestep["time"] = switches.interpolation_point_at_index(index);
      std::vector<double> switchvalues(
          static_cast<size_t>(get_number_of_switches_per_timepoint()));

      auto startindex = get_switch_startindex();
      for (size_t value_index = 0; value_index != switchvalues.size();
           ++value_index) {
        auto sindex = static_cast<Eigen::Index>(value_index);
        switchvalues[value_index]
            = switches.vector_at_index(index)[startindex + sindex];
      }
      this_timestep["values"] = switchvalues;
      output_json["data"].push_back(std::move(this_timestep));
    }
    json[this_class][this_type].push_back(std::move(output_json));
  }

} // namespace Model
