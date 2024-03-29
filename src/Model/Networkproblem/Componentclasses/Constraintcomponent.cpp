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
#include "Constraintcomponent.hpp"
#include "Exception.hpp"

namespace Model {

  Eigen::Index
  Constraintcomponent::get_number_of_constraints_per_timepoint() const {
    return get_constraint_afterindex() - get_constraint_startindex();
  }

  /// \brief getter for #start_constraint_index
  Eigen::Index Constraintcomponent::get_constraint_startindex() const {
    if (constraint_startindex < 0) {
      gthrow(
          {"start_constraint_index < 0. Probably get_number_of_constraints() "
           "was called "
           "before calling set_constraint_indices().\n This is forbidden."});
    }

    return constraint_startindex;
  }

  /// \brief getter for #after_constraint_index
  Eigen::Index Constraintcomponent::get_constraint_afterindex() const {
    if (constraint_afterindex < 0) {
      gthrow(
          {"after_constraint_index < 0. Probably get_number_of_constraints() "
           "was called "
           "before calling set_constraint_indices().\n This is forbidden."});
    }

    return constraint_afterindex;
  }
} // namespace Model
