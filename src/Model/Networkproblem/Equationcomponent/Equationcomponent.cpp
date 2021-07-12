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

#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <map>

namespace Model::Networkproblem {

  void Equationcomponent::prepare_timestep(
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> // last_state
      ,
      Eigen::Ref<Eigen::VectorXd const> // new_state
  ) {}

  void Equationcomponent::setup() {}

  std::optional<nlohmann::json> Equationcomponent::get_boundary_schema() {
    return std::nullopt;
  }

  std::optional<nlohmann::json> Equationcomponent::get_control_schema() {
    return std::nullopt;
  }

} // namespace Model::Networkproblem
