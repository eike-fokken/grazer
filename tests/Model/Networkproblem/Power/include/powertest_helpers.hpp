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

#pragma once
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

nlohmann::json powernode_json(
    std::string id, double G, double B, Eigen::Vector2d bd0,
    Eigen::Vector2d bd1);
nlohmann::json transmissionline_json(
    std::string id, double G, double B, nlohmann::json const &startnode,
    nlohmann::json const &endnode);
