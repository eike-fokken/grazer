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
#include "InterpolatingVector.hpp"

namespace Optimization {

  struct Initialvalues {
    Initialvalues(
        Aux::InterpolatingVector initial_controls,
        Aux::InterpolatingVector lower_bounds,
        Aux::InterpolatingVector upper_bounds,
        Aux::InterpolatingVector constraint_lower_bounds,
        Aux::InterpolatingVector constraint_upper_bounds);
    Aux::InterpolatingVector const initial_controls;
    Aux::InterpolatingVector const lower_bounds;
    Aux::InterpolatingVector const upper_bounds;
    Aux::InterpolatingVector const constraint_lower_bounds;
    Aux::InterpolatingVector const constraint_upper_bounds;
  };
} // namespace Optimization
