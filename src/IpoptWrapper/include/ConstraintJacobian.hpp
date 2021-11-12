#pragma once
#include "InterpolatingVector.hpp"
#include <Eigen/Dense>
#include <Eigen/src/Core/util/Meta.h>
#include <IpTypes.hpp>
#include <vector>

namespace Optimization {

  /** @brief A class holding the structure of a Constraint Jacobian.
   *
   * This class is suitable if the Jacobian is "half-dense", meaning, that each
   * row of the Jacobian has non-zero entries in the first column and all
   * following columns up until a row-dependent maximal index, after which only
   * zeros follow. The maximal index is constant and may not change after
   * construction. Neither may the number of rows and columns.
   */
  class ConstraintJacobian {
  public:
    ConstraintJacobian(
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

    double &CoeffRef(
        Ipopt::Number *values, Ipopt::Index values_end, Eigen::Index row,
        Eigen::Index col);

    double Coeff(
        Ipopt::Number *values, Ipopt::Index values_end, Eigen::Index row,
        Eigen::Index col) const;

    Eigen::Map<Eigen::VectorXd> const
    row(Ipopt::Number *values, Ipopt::Index values_end, Eigen::Index row_index);
    Eigen::Map<Eigen::VectorXd const> const
    row(Ipopt::Number *values, Ipopt::Index values_end,
        Eigen::Index row_index) const;

    /** @brief number of non-zeros in the given row.
     */
    Eigen::Index size_of_row(Eigen::Index row);

    /** @brief number of rows in the matrix.
     */
    Eigen::Index rows() const;
    /** @brief number of columns in the matrix.
     */
    Eigen::Index cols() const;

    /** @brief Number of non-zero elements.
     */
    Eigen::Index nonZeros() const;

  private:
    /** @brief Number of columns in the matrix.
     */
    Eigen::Index number_of_columns;

    /** @brief For each row this vector holds the starting index
     */
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> start_of_rows;

    /** @brief length of the values array.
     */
    Ipopt::Index nnz;
  };

} // namespace Optimization
