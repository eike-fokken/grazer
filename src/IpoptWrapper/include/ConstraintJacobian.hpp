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
  class MappedConstraintJacobian {
  public:
    static MappedConstraintJacobian make_instance(
        double *values, Eigen::Index number_of_entries,
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

    Eigen::Ref<Eigen::MatrixXd> get_nnz_column_block(Eigen::Index column);
    Eigen::Ref<Eigen::MatrixXd const>
    get_nnz_column_block(Eigen::Index column) const;

    Eigen::Index nonZeros() const;

    MappedConstraintJacobian(
        double *values, Eigen::Index number_of_entries,
        Eigen::Index number_of_rows_per_block,
        Eigen::Index number_of_columns_per_block,
        Eigen::Index number_of_column_blocks, Eigen::Index number_of_row_blocks,
        Eigen::Vector<Eigen::Index, Eigen::Dynamic> column_block_starts);

    double *values;
    Eigen::Index number_of_entries;
    Eigen::Index const number_of_rows_per_block;
    Eigen::Index const number_of_columns_per_block;
    Eigen::Index const number_of_column_blocks;
    Eigen::Index const number_of_row_blocks;
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> const column_block_starts;
  };

  class ConstraintJacobian {
  public:
    static ConstraintJacobian make_instance(
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls);

  private:
    ConstraintJacobian(
        Eigen::Index number_of_entries, Eigen::Index number_of_rows_per_block,
        Eigen::Index number_of_columns_per_block,
        Eigen::Index number_of_column_blocks, Eigen::Index number_of_row_blocks,
        Eigen::Vector<Eigen::Index, Eigen::Dynamic> column_block_starts);

    Eigen::Ref<Eigen::MatrixXd> get_nnz_column_block(Eigen::Index column);
    Eigen::Ref<Eigen::MatrixXd const>
    get_nnz_column_block(Eigen::Index column) const;
    Eigen::Index nonZeros() const;

    Eigen::Vector<double, Eigen::Dynamic> data;
    MappedConstraintJacobian jac;
  };

} // namespace Optimization
