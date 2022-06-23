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
#include <Exception.hpp>
#include <Mathfunctions.hpp>
#include <cmath>

namespace Aux {

  double epsilon = 1e-4;

  double const Pi = 3.141592653589793;
  double const EPSILON = 1e-10;

  double circle_area(double radius) { return Pi * radius * radius; }

  double smooth_abs(double x) {

    if (x <= -epsilon) {
      return -x;
    } else if (x >= epsilon) {
      return x;
    } else {
      double rel = x / epsilon;
      return x * rel / 2.0 * (-rel * rel + 3);
    }
  }

  double dsmooth_abs_dx(double x) {

    if (x <= -epsilon) {
      return -1.0;
    } else if (x >= epsilon) {
      return 1.0;
    } else {
      double rel = x / epsilon;
      return rel * (-2 * rel * rel + 3);
    }
  }

  int dabs_dx(double val) {
    if (std::abs(val) < 1e-13) {
      gthrow({"The derivative of the absolute value at 0 is undefined!"});
    }
    return (0 < val) - (val < 0);
  }

} // namespace Aux
