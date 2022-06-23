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
#pragma once
#include "InterpolatingVector.hpp"
#include <Eigen/Dense>
#include <vector>

namespace Optimization {
  struct Cacheentry {
    Aux::InterpolatingVector control;
    Eigen::VectorXd state_timepoints;
    Eigen::VectorXd initial_state;
  };
} // namespace Optimization
