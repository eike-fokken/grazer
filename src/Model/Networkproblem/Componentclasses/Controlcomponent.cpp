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
#include "Controlcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include <nlohmann/json.hpp>

namespace Model {
  void Controlcomponent::prepare_timestep(
      double /*last_time*/, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*control*/) {}

  Eigen::Index Controlcomponent::get_number_of_controls_per_timepoint() const {
    return get_control_afterindex() - get_control_startindex();
  }

  Eigen::Index Controlcomponent::get_control_startindex() const {
    if (control_startindex < 0) {
      gthrow(
          {"control_startindex < 0. Probably ", __func__, " was called ",
           "before calling set_indices().\n This is forbidden."});
    }
    return control_startindex;
  }
  Eigen::Index Controlcomponent::get_control_afterindex() const {
    if (control_afterindex < 0) {
      gthrow(
          {"control_afterindex < 0. Probably ", __func__,
           " was called "
           "before calling set_indices().\n This is forbidden."});
    }
    return control_afterindex;
  }

} // namespace Model
