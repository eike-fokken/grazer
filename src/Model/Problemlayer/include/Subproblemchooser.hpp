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
#include "Subproblem.hpp"
#include <memory>
#include <nlohmann/json.hpp>

namespace Model {

  std::unique_ptr<Subproblem> build_subproblem(
      std::string subproblem_type, nlohmann::json &subproblem_json);

  std::unique_ptr<Subproblem>
  build_networkproblem(nlohmann::json &subproblem_json);
} // namespace Model
