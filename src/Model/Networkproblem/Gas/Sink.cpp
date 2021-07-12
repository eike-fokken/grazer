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

#include "Sink.hpp"
#include "Exception.hpp"

namespace Model::Networkproblem::Gas {

  std::string Sink::get_type() { return "Sink"; }

  Sink::Sink(nlohmann::json const &data) :
      Flowboundarynode(revert_boundary_conditions(data)) {}

  nlohmann::json Sink::revert_boundary_conditions(nlohmann::json const &data) {

    nlohmann::json reverted_boundary_json = data;
    if (not reverted_boundary_json.contains("boundary_values")) {
      gthrow(
          {"The sink json ", data["id"],
           " did not contain boundary values.  Something went wrong.\n",
           "Its json was: ", data.dump(1, '\t')});
    }
    for (auto &datum : reverted_boundary_json["boundary_values"]["data"]) {
      for (auto &value_json : datum["values"]) {
        double old_value = value_json.get<double>();
        value_json = -old_value;
      }
    }

    return reverted_boundary_json;
  }

} // namespace Model::Networkproblem::Gas
