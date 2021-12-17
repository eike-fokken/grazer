#pragma once
#include "InterpolatingVector.hpp"
#include <Eigen/Dense>
#include <Eigen/src/Core/Matrix.h>
#include <Eigen/src/Core/util/Constants.h>
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

    Eigen::MatrixXd &matrixRef();
    Eigen::MatrixXd const &matrix() const;

    Eigen::Ref<Eigen::MatrixXd>
    get_nnz_column_block(Eigen::Index block_row_start);
    Eigen::Ref<Eigen::MatrixXd const>
    get_nnz_column_block(Eigen::Index block_row_start) const;

    Eigen::Index nonZeros() const;

  private:
    /** @brief The constructor is private so we can make sure the right
     * structure is passed in the make_instance method.
     */
    ConstraintJacobian(
        Eigen::Index number_of_rows_per_block,
        Eigen::Index number_of_columns_per_block,
        Eigen::Index number_of_column_blocks, Eigen::Index number_of_row_blocks,
        Eigen::Vector<Eigen::Index, Eigen::Dynamic> column_block_starts);

    Eigen::MatrixXd jacobian;
    Eigen::Index const number_of_rows_per_block;
    Eigen::Index const number_of_columns_per_block;
    Eigen::Index const number_of_column_blocks;
    Eigen::Index const number_of_row_blocks;
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> const column_block_starts;
  };

} // namespace Optimization
