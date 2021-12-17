#include "ConstraintJacobian.hpp"
#include "Exception.hpp"
#include <algorithm>
#include <cstddef>

namespace Optimization {

  ConstraintJacobian::ConstraintJacobian(
      Eigen::Index _number_of_rows_per_block,
      Eigen::Index _number_of_columns_per_block,
      Eigen::Index _number_of_column_blocks, Eigen::Index _number_of_row_blocks,
      Eigen::Vector<Eigen::Index, Eigen::Dynamic> _column_block_starts) :
      number_of_rows_per_block(_number_of_rows_per_block),
      number_of_columns_per_block(_number_of_columns_per_block),
      number_of_column_blocks(_number_of_column_blocks),
      number_of_row_blocks(_number_of_row_blocks),
      column_block_starts(std::move(_column_block_starts)) {}

  Eigen::MatrixXd &ConstraintJacobian::matrixRef() { return jacobian; }
  Eigen::MatrixXd const &ConstraintJacobian::matrix() const { return jacobian; }

  Eigen::Ref<Eigen::MatrixXd>
  ConstraintJacobian::get_nnz_column_block(Eigen::Index column) {

    auto colstart = column * number_of_columns_per_block;
    auto rowstart = column_block_starts[column] * number_of_rows_per_block;
    auto number_of_rows
        = (number_of_row_blocks - rowstart) * number_of_rows_per_block;
    assert(false); // check this function!
    return matrix().block(
        rowstart, colstart, number_of_rows, number_of_columns_per_block);
  }

  Eigen::Ref<Eigen::MatrixXd const>
  ConstraintJacobian::get_nnz_column_block(Eigen::Index column) const {
    auto colstart = column * number_of_columns_per_block;
    auto rowstart = column_block_starts[column] * number_of_rows_per_block;
    auto number_of_rows
        = (number_of_row_blocks - rowstart) * number_of_rows_per_block;
    assert(false); // check this function!
    return matrix().block(
        rowstart, colstart, number_of_rows, number_of_columns_per_block);
  }

  Eigen::Index ConstraintJacobian::nonZeros() const {
    auto blocksize = number_of_rows_per_block * number_of_columns_per_block;
    Eigen::Index no_of_zeros = 0;
    for (auto const &rowstart : column_block_starts) {
      no_of_zeros += rowstart * blocksize;
    }
    return matrix().rows() * (matrix().cols()) - no_of_zeros;
  }

  ConstraintJacobian ConstraintJacobian::make_instance(
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls) {

    Eigen::Index number_of_row_blocks = constraints.size();
    Eigen::Index number_of_constraints_per_step
        = constraints.get_inner_length();
    Eigen::Index number_of_controls_per_step = controls.get_inner_length();
    Eigen::Index number_of_column_blocks = controls.size();

    Eigen::Vector<Eigen::Index, Eigen::Dynamic> column_block_starts(
        number_of_column_blocks);

    // as we haven't programmed iterators for InterpolatingVector, this is our
    // way of iterating in reverse:
    for (Eigen::Index icontrol = number_of_column_blocks - 1; icontrol != -1;
         --icontrol) {

      if (icontrol == 0) {
        column_block_starts[icontrol] = 0;
        break;
      }

      // auto controltime = controls.interpolation_point_at_index(icontrol);
      auto previous_controltime
          = controls.interpolation_point_at_index(icontrol - 1);

      auto &constrainttimes = constraints.get_interpolation_points();

      auto greater_than
          = [](double const &a, double const &b) -> bool { return a > b; };
      // go from highest time point of constrainttimes to lowest and find the
      // first time that is lesser of equal to the control timepoint one less.
      //
      // This is because  that is the last contraint time that cannot depend on
      // the control at controltime.
      auto constraint_time_iterator = std::lower_bound(
          constrainttimes.rbegin(), constrainttimes.rend(),
          previous_controltime, greater_than);
      auto first_dependent_constraint_it = std::next(constraint_time_iterator);
      auto first_relevant_block_row_index
          = (constrainttimes.rend() - first_dependent_constraint_it - 1);
      column_block_starts[icontrol] = first_relevant_block_row_index;
    }

    return ConstraintJacobian(
        number_of_constraints_per_step, number_of_controls_per_step,
        number_of_column_blocks, number_of_row_blocks,
        std::move(column_block_starts));
  }

} // namespace Optimization
