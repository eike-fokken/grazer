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

#pragma once
namespace Aux {

  /// \brief The mathematical constant Pi
  extern double const Pi;

  /// \brief This constant is chosen such that doubles whose differences are
  /// less than EPSILON, are considered equal.
  extern double const EPSILON;

  double circle_area(double radius);

  /// A C^1 absolute value function.
  double smooth_abs(double x);

  /// The derivative of smooth_abs.
  double dsmooth_abs_dx(double x);

  /// This is a derivative function for double std::abs(double x).
  /// It throws an exception, if it is called on a value smaller than 1e-13.
  int dabs_dx(double x);

} // namespace Aux
