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

#include "powertest_helpers.hpp"
#include "Equationcomponent_test_helpers.hpp"

nlohmann::json powernode_json(
    std::string id, double G, double B, Eigen::Vector2d bd0,
    Eigen::Vector2d bd1) {
  nlohmann::json powernode_json;
  powernode_json["id"] = id;
  powernode_json["G"] = G;
  powernode_json["B"] = B;
  auto b0 = make_value_json(id, "time", bd0, bd1);
  powernode_json["boundary_values"] = b0;
  return powernode_json;
}

nlohmann::json transmissionline_json(
    std::string id, double G, double B, nlohmann::json const &startnode,
    nlohmann::json const &endnode) {
  nlohmann::json transmissionline_json;
  transmissionline_json["id"] = id;
  transmissionline_json["G"] = G;
  transmissionline_json["B"] = B;
  transmissionline_json["from"] = startnode["id"];
  transmissionline_json["to"] = endnode["id"];

  return transmissionline_json;
}
