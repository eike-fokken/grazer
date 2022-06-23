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
#include "Costgradienthandler.hpp"
#include "Exception.hpp"

namespace Aux {

  Costgradienthandler::Costgradienthandler(
      Eigen::Ref<Eigen::VectorXd> _gradient) :
      gradient(_gradient) {

#ifndef NDEBUG
    if (!_gradient.isZero()) {
      gthrow({"Costgradienthandler was given a non-Zero array!"});
    }
#endif
  }

  void Costgradienthandler::add_to_coefficient(int row, double value) {
    gradient[row] += value;
  }
} // namespace Aux
