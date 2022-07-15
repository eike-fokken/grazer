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
#include "Costcomponent.hpp"

namespace Model {
  Costcomponent::Costcomponent() {}
  Costcomponent::Costcomponent(double _cost_weight) :
      cost_weight(_cost_weight) {}

  Costcomponent::Costcomponent(double _cost_weight, double _penalty_weight) :
      cost_weight(_cost_weight), penalty_weight(_penalty_weight) {}

  double Costcomponent::get_cost_weight() const { return cost_weight; }
  double Costcomponent::get_penalty_weight() const { return penalty_weight; }

} // namespace Model
