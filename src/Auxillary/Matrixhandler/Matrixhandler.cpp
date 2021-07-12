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

#include <Eigen/Sparse>
#include <Matrixhandler.hpp>

namespace Aux {

  void Triplethandler::add_to_coefficient(int row, int col, double value) {
    Eigen::Triplet<double> newtriplet(row, col, value);
    tripletlist.push_back(newtriplet);
  }

  void Triplethandler::set_coefficient(int row, int col, double value) {
    add_to_coefficient(row, col, value);
  }

  void Triplethandler::set_matrix() {
    matrix->setFromTriplets(tripletlist.begin(), tripletlist.end());
    tripletlist.clear();
  }

  void Coeffrefhandler::set_coefficient(int row, int col, double value) {
    matrix->coeffRef(row, col) = value;
  }
  void Coeffrefhandler::add_to_coefficient(int row, int col, double value) {
    matrix->coeffRef(row, col) += value;
  }

  void Coeffrefhandler::set_matrix() {}

} // namespace Aux
