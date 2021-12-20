#include "ConstraintJacobian.hpp"
#include "Exception.hpp"
#include <Eigen/Dense>
#include <Eigen/src/Core/Map.h>
#include <Eigen/src/Core/Matrix.h>
#include <IpTypes.hpp>
#include <algorithm>
#include <cstddef>
#include <utility>

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
        _block_column_offsets[control_index]
            = _block_column_offsets[control_index - 1] + constraint_index;
        last_controltime = controltimes[control_index];
        ++control_index;
      } else {
        ++constraint_index;
      }
    }

    return ConstraintJacobian_impl(
        block_width, block_height, std::move(_block_column_offsets));
  }

  void ConstraintJacobian_impl::supply_indices(
      Ipopt::Index *iRow, Ipopt::Index *jCol,
      [[maybe_unused]] Eigen::Index number_of_values) const {

    Ipopt::Index index = 0;
    for (Eigen::Index j = 0; j != get_outer_width(); ++j) {
      auto columnoffset = get_inner_colstart_jacobian(j);
      auto rowoffset = get_inner_rowstart_jacobian(j);

      for (Eigen::Index innercol = columnoffset; innercol != get_block_width();
           ++innercol) {
        for (Eigen::Index innerrow = rowoffset; innerrow != get_inner_height();
             ++innerrow) {
          assert(index < number_of_values);
          iRow[index] = static_cast<int>(innerrow);
          jCol[index] = static_cast<int>(innercol);
          ++index;
        }
      }
    }
  }

  ConstraintJacobian_impl::ConstraintJacobian_impl(
      Eigen::Index _block_width, Eigen::Index _block_height,
      Eigen::Vector<Eigen::Index, Eigen::Dynamic> _block_column_offsets) :
      block_width(_block_width),
      block_height(_block_height),
      block_column_offsets(std::move(_block_column_offsets)) {}

  Eigen::Index ConstraintJacobian_impl::get_block_width() const {
    return block_width;
  }
  Eigen::Index ConstraintJacobian_impl::get_block_height() const {
    return block_height;
  }
  Eigen::Index ConstraintJacobian_impl::get_block_size() const {
    return get_block_height() * get_block_width();
  }
  Eigen::Index ConstraintJacobian_impl::get_outer_width() const {
    return block_column_offsets.size() - 1;
  }
  Eigen::Index ConstraintJacobian_impl::get_inner_width() const {
    return get_outer_width() * get_block_width();
  }
  Eigen::Index ConstraintJacobian_impl::get_outer_height() const {
    return block_column_offsets[1];
  }
  Eigen::Index ConstraintJacobian_impl::get_inner_height() const {
    return get_outer_height() * get_block_height();
  }
  Eigen::Index
  ConstraintJacobian_impl::get_outer_col_height(Eigen::Index column) const {
    assert(column >= 0);
    assert(column < get_outer_width());
    return block_column_offsets[column + 1] - block_column_offsets[column];
  }
  Eigen::Index
  ConstraintJacobian_impl::get_inner_col_height(Eigen::Index column) const {
    assert(column >= 0);
    assert(column < get_outer_width());
    return get_outer_col_height(column) * get_block_height();
  }

  Eigen::Index ConstraintJacobian_impl::get_outer_rowstart_jacobian(
      Eigen::Index column) const {
    assert(column >= 0);
    assert(column < get_outer_width());
    return get_outer_height() - get_outer_col_height(column);
  }
  Eigen::Index ConstraintJacobian_impl::get_inner_rowstart_jacobian(
      Eigen::Index column) const {
    return get_outer_rowstart_jacobian(column) * get_block_height();
  }
  Eigen::Index ConstraintJacobian_impl::get_outer_colstart_jacobian(
      Eigen::Index column) const {
    return column;
  }
  Eigen::Index ConstraintJacobian_impl::get_inner_colstart_jacobian(
      Eigen::Index column) const {
    return get_outer_colstart_jacobian(column) * get_block_width();
  }

  Eigen::Index
  ConstraintJacobian_impl::column_values_start(Eigen::Index column) const {
    return get_block_size() * block_column_offsets[column];
  }
  Eigen::Index
  ConstraintJacobian_impl::column_values_end(Eigen::Index column) const {
    return block_column_offsets[column + 1] * get_block_size();
  }

  Eigen::Ref<Eigen::MatrixXd> ConstraintJacobian_impl::get_nnz_column_block(
      double *values, Eigen::Index number_of_entries, Eigen::Index column) {
    assert(false); // weiter!
  }
  Eigen::Ref<Eigen::MatrixXd const>
  ConstraintJacobian_impl::get_nnz_column_block(
      double *values, Eigen::Index number_of_entries,
      Eigen::Index column) const {
    assert(false); // weiter!
  }

  Eigen::Index ConstraintJacobian_impl::nonZeros_of_structure() const {
    return block_column_offsets[block_column_offsets.size() - 1];
  }

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

  ConstraintJacobian::ConstraintJacobian(
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls) :
      impl(ConstraintJacobian_impl::make_instance(constraints, controls)),
      storage(impl.nonZeros_of_structure()) {}

  Eigen::Ref<Eigen::MatrixXd>
  ConstraintJacobian::get_nnz_column_block(Eigen::Index column) {
    return impl.get_nnz_column_block(storage.data(), storage.size(), column);
  }
  Eigen::Ref<Eigen::MatrixXd const>
  ConstraintJacobian::get_nnz_column_block(Eigen::Index column) const {
    return impl.get_nnz_column_block(storage.data(), storage.size(), column);
  }
  Eigen::Index ConstraintJacobian::nonZeros() const { return storage.size(); }

  void ConstraintJacobian::setZero() {}

} // namespace Optimization
