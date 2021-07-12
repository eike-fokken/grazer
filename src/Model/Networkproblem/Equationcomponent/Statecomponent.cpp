/*
 * Grazer - network simulation tool
 *
 * Copyright 2020-2021 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	AGPL-3.0-or-later
 *
 * Licensed under the GNU Affero General Public License v3.0, found in
 * LICENSE.txt and at https://www.gnu.org/licenses/agpl-3.0.html
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */

#include "Statecomponent.hpp"
#include "Exception.hpp"

namespace Model::Networkproblem {

  nlohmann::json Statecomponent::get_initial_schema() {
    gthrow(
        {"This static method must be implemented in the class inheriting "
         "from Equationcomponent!"});
  }

  void Statecomponent::setup_output_json_helper(std::string const &id) {
    auto &output_json = get_output_json_ref();
    output_json["id"] = id;
    output_json["data"] = nlohmann::json::array();
  }

  int Statecomponent::set_indices(int const next_free_index) {
    start_state_index = next_free_index;
    int number_of_states = get_number_of_states();
    after_state_index = next_free_index + number_of_states;

    return after_state_index;
  }

  int Statecomponent::get_start_state_index() const {
    return start_state_index;
  }
  int Statecomponent::get_after_state_index() const {
    return after_state_index;
  }

  nlohmann::json &Statecomponent::get_output_json_ref() {
    return component_output;
  }

  void Statecomponent::print_to_files(nlohmann::json &) {}
} // namespace Model::Networkproblem
