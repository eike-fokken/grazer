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
    /** @brief Returns a matrix that contains 1 at all points where a constraint
     * jacobian can be non-zero and zero everywhere else.
     */
    static ConstraintJacobian make_instance(
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

    Eigen::Vector<Eigen::Index, Eigen::Dynamic> const &get_start_of_rows() {
      return start_of_rows;
    }

  private:
    /** @brief The constructor is private so we can make sure the right
     * structure is passed in the make_instance method.
     */
    ConstraintJacobian(
        Eigen::Vector<Eigen::Index, Eigen::Dynamic> start_indices_of_rows,
        Eigen::Index number_of_columns);

    /** @brief For each row this vector holds the starting index. The last entry
     * marks the end index of the last row and is also the total number of
     * non-zeros in the jacobian.
     */
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> const start_of_rows;
    /** @brief Number of columns in the matrix.
     */
    Eigen::Index const number_of_columns;
  };

} // namespace Optimization
