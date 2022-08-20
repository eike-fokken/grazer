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
#include "Matrixhandler.hpp"
#include <Eigen/Sparse>

namespace Aux {
  Matrixhandler::~Matrixhandler() {}

  template <int Transpose>
  Triplethandler<Transpose>::Triplethandler(
      Eigen::SparseMatrix<double> &_matrix) :
      Matrixhandler(_matrix) {
    assert(_matrix.nonZeros() == 0);
  }

  template <int Transpose>
  void Triplethandler<Transpose>::add_to_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {
    Eigen::Index actual_row = -1;
    Eigen::Index actual_col = -1;
    if constexpr (not Transpose) {
      actual_row = row;
      actual_col = col;
    } else {
      actual_row = col;
      actual_col = row;
    }
    assert(0 <= actual_row);
    assert(actual_row < matrix.rows());
    assert(0 <= actual_col);
    assert(actual_col < matrix.cols());
    Eigen::Triplet<double, Eigen::Index> newtriplet
        = {actual_row, actual_col, value};
    tripletlist.push_back(newtriplet);
  }
  template <int Transpose> void Triplethandler<Transpose>::set_matrix() {
    matrix.setFromTriplets(tripletlist.begin(), tripletlist.end());
    tripletlist.clear();
  }

  template <int Transpose>
  Coeffrefhandler<Transpose>::Coeffrefhandler(
      Eigen::SparseMatrix<double> &_matrix) :
      Matrixhandler(_matrix) {
    // Set the matrix to zero:
    Eigen::Map<Eigen::VectorXd> coefficients(
        _matrix.valuePtr(), _matrix.nonZeros());
    coefficients.setZero();
  }

  template <int Transpose>
  void Coeffrefhandler<Transpose>::add_to_coefficient(
      Eigen::Index row, Eigen::Index col, double value) {

    Eigen::Index actual_row = -1;
    Eigen::Index actual_col = -1;
    if constexpr (not Transpose) {
      actual_row = row;
      actual_col = col;
    } else {
      actual_row = col;
      actual_col = row;
    }
    assert(0 <= actual_row);
    assert(actual_row < matrix.rows());
    assert(0 <= actual_col);
    assert(actual_col < matrix.cols());
    matrix.coeffRef(actual_row, actual_col) += value;
  }

  template <int Transpose> void Coeffrefhandler<Transpose>::set_matrix() {}

  template class Triplethandler<Transposed>;
  template class Triplethandler<Regular>;

  template class Coeffrefhandler<Transposed>;
  template class Coeffrefhandler<Regular>;

} // namespace Aux
