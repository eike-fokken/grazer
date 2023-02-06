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
#include "Statecomponent.hpp"
#include "Exception.hpp"

namespace Model {

  void Statecomponent::setup_output_json_helper(std::string const &id) {
    auto &output_json = get_output_json_ref();
    output_json["id"] = id;
    output_json["data"] = nlohmann::json::array();
  }

  Eigen::Index Statecomponent::get_number_of_states() const {
    return get_state_afterindex() - get_state_startindex();
  }

  nlohmann::json &Statecomponent::get_output_json_ref() {
    return component_output;
  }

} // namespace Model
