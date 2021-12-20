#include "ConstraintJacobian.hpp"
#include "Exception.hpp"
#include <algorithm>
#include <cstddef>

static size_t get_alignment_lesser_equal_1024(void *ptr) {
  std::size_t space = 1u;
  size_t alignment = 2048;
  void *result = nullptr;
  while ((not result) and alignment != 1) {
    alignment /= 2;
    result = std::align(alignment, 1u, ptr, space);
  }
  return alignment;
}

namespace Optimization {

  ConstraintJacobian_impl ConstraintJacobian_impl::make_instance(
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls) {
    Eigen::Index block_height = constraints.get_inner_length();
    Eigen::Index block_width = controls.get_inner_length();
    Eigen::Index number_of_column_blocks = controls.size();

    Eigen::Vector<Eigen::Index, Eigen::Dynamic> _block_column_offsets(
        number_of_column_blocks + 1);

    Eigen::Index constraint_index = 0;
    auto const &constrainttimes = constraints.get_interpolation_points();
    Eigen::Index control_index = 1;
    auto const &controltimes = controls.get_interpolation_points();

    auto last_controltime = controltimes[0];
    _block_column_offsets[0] = 0;
    while (control_index != controltimes.size()) {
      if (constrainttimes[constraint_index] > last_controltime) {
        _block_column_offsets[control_index] = constraint_index;
        last_controltime = controltimes[control_index];
        ++control_index;
        continue;
      } else {
        ++constraint_index;
      }
    }

    for (Eigen::Index icontrol = number_of_column_blocks - 1; icontrol != -1;
         --icontrol) {

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
      auto first_dependent_constraint_it = std::prev(constraint_time_iterator);
      auto first_relevant_block_row_index
          = (constrainttimes.rend() - first_dependent_constraint_it - 1);
      _block_column_offsets[icontrol] = first_relevant_block_row_index;
    }

    return ConstraintJacobian_impl(
        block_width, block_height, std::move(_block_column_offsets));
  }

  ConstraintJacobian_impl::ConstraintJacobian_impl(
      Eigen::Index _block_width, Eigen::Index _block_height,
      Eigen::Vector<Eigen::Index, Eigen::Dynamic> _block_column_offsets) :
      block_width(_block_width),
      block_height(_block_height),
      block_column_offsets(std::move(_block_column_offsets)) {}

  Eigen::Ref<Eigen::MatrixXd>
  MappedConstraintJacobian::get_nnz_column_block(Eigen::Index column) {

    return impl.get_nnz_column_block(values, number_of_entries, column);
  }

  Eigen::Ref<Eigen::MatrixXd const>
  MappedConstraintJacobian::get_nnz_column_block(Eigen::Index column) const {
    return impl.get_nnz_column_block(values, number_of_entries, column);
  }

  Eigen::Index MappedConstraintJacobian::nonZeros() const {
    return number_of_entries;
  }

  void MappedConstraintJacobian::setZero() {
    if (get_alignment_lesser_equal_1024(values) >= 16) {
      Eigen::Map<Eigen::VectorXd, Eigen::Aligned16> map(values, nonZeros());
      map.setZero();
    } else {
      Eigen::Map<Eigen::VectorXd> map(values, nonZeros());
      map.setZero();
    }
  }

  //

  /////////////////////////////////////////////////////////
  // ConstraintJacobian:
  /////////////////////////////////////////////////////////

  ConstraintJacobian ConstraintJacobian::make_instance(
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls) {
    static_assert(
        false, "write this function, maybe factor out some logic from above!");
  }

  ConstraintJacobian::ConstraintJacobian(
      Eigen::Index number_of_entries, Eigen::Index number_of_rows_per_block,
      Eigen::Index number_of_columns_per_block,
      Eigen::Index number_of_column_blocks, Eigen::Index number_of_row_blocks,
      Eigen::Vector<Eigen::Index, Eigen::Dynamic> column_block_starts) :
      data(number_of_entries),
      jac(data.data(), number_of_entries, number_of_rows_per_block,
          number_of_columns_per_block, number_of_column_blocks,
          number_of_row_blocks, std::move(column_block_starts)) {}

  Eigen::Ref<Eigen::MatrixXd>
  ConstraintJacobian::get_nnz_column_block(Eigen::Index column) {
    return jac.get_nnz_column_block(column);
  }
  Eigen::Ref<Eigen::MatrixXd const>
  ConstraintJacobian::get_nnz_column_block(Eigen::Index column) const {
    return jac.get_nnz_column_block(column);
  }
  Eigen::Index ConstraintJacobian::nonZeros() const { return jac.nonZeros(); }

  void ConstraintJacobian::setZero() { jac.setZero(); }

} // namespace Optimization
