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
#include <Eigen/Sparse>
#include <vector>

namespace Aux {

  enum Transpose { Regular = 0, Transposed = 1 };

  /// \brief A utility base class that is used to setup and modify a sparse
  /// matrix.
  class Matrixhandler {

  public:
    Matrixhandler() = delete;
    Matrixhandler(Eigen::SparseMatrix<double> &_matrix) : matrix(_matrix) {};

    virtual ~Matrixhandler();

    /// \brief Adds to a coefficient.
    ///
    /// @param row The row index of the coefficient.
    /// @param col The column index of the coefficient.
    /// @param value The value to be added to the already present coefficient.
    virtual void
    add_to_coefficient(Eigen::Index row, Eigen::Index col, double value)
        = 0;

    /// For #Triplethandler: Builds the matrix from the gathered coefficients
    /// and then forgets the coefficients.
    ///
    /// For #Coeffrefhandler: Does nothing.
    virtual void set_matrix() = 0;

  protected:
    Eigen::SparseMatrix<double> &matrix;
  };

  /// \brief The Triplethandler variety gathers the coefficients in triplets and
  /// later builds the matrix from the triplets. It works only on an empty
  /// matrix.
  template <int Transpose = Regular>
  class Triplethandler final : public Matrixhandler {

  public:
    Triplethandler(Eigen::SparseMatrix<double> &matrix);

    void
    add_to_coefficient(Eigen::Index row, Eigen::Index col, double value) final;

    void set_matrix() final;

  private:
    std::vector<Eigen::Triplet<double, Eigen::Index>> tripletlist;
  };

  /// \brief The Coeffrefhandler variety directly sets the coefficients and
  /// contains no state. It sets the matrix to zero on construction.

  template <int Transpose = Regular>
  class Coeffrefhandler final : public Matrixhandler {

  public:
    Coeffrefhandler(Eigen::SparseMatrix<double> &matrix);

    void
    add_to_coefficient(Eigen::Index row, Eigen::Index col, double value) final;

    void set_matrix() final;
  };

  extern template class Triplethandler<Regular>;
  extern template class Coeffrefhandler<Regular>;

  extern template class Triplethandler<Transposed>;
  extern template class Coeffrefhandler<Transposed>;

} // namespace Aux
