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

#include <Exception.hpp>
#include <Subproblem.hpp>

namespace Model {

  int Subproblem::get_number_of_states() const {
    int int_start = start_state_index;
    int int_after = after_state_index;

    // Be careful: We expect number of states to be GREATER than zero.
    if (int number_of_states = int_after - int_start > 0) {
      return number_of_states;
    } else {
      gthrow({"Number of states is negative or zero!"});
    }
  }

  int Subproblem::set_indices(int const next_free_index) {
    start_state_index = next_free_index;
    after_state_index = reserve_indices(next_free_index);
    return after_state_index;
  }

  int Subproblem::get_start_state_index() const { return start_state_index; }
  int Subproblem::get_after_state_index() const { return after_state_index; }

} // namespace Model
