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
#include "Initialvalues.hpp"

namespace Optimization {

  Initialvalues::Initialvalues(
      Aux::InterpolatingVector _initial_controls,
      Aux::InterpolatingVector _lower_bounds,
      Aux::InterpolatingVector _upper_bounds,
      Aux::InterpolatingVector _constraint_lower_bounds,
      Aux::InterpolatingVector _constraint_upper_bounds) :
      initial_controls(std::move(_initial_controls)),
      lower_bounds(std::move(_lower_bounds)),
      upper_bounds(std::move(_upper_bounds)),
      constraint_lower_bounds(std::move(_constraint_lower_bounds)),
      constraint_upper_bounds(std::move(_constraint_upper_bounds)) {}
} // namespace Optimization
