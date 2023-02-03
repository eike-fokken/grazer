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
#include "ConstraintJacobian.hpp"
#include "Exception.hpp"
#include <Eigen/Dense>
#include <IpTypes.hpp>
#include <cassert>

namespace Optimization {

  std::tuple<Eigen::Index, Eigen::Index, Eigen::VectorX<Eigen::Index>>
  make_data(
      Eigen::Index number_of_constraints_per_step,
      Eigen::Index number_of_controls_per_step,
      Eigen::Ref<Eigen::VectorXd const> const &constraint_interpolationpoints,
      Eigen::Ref<Eigen::VectorXd const> const &control_interpolationpoints) {
    Eigen::Index block_height = number_of_constraints_per_step;
    Eigen::Index block_width = number_of_controls_per_step;
    Eigen::Index number_of_column_blocks = control_interpolationpoints.size();

    Eigen::Vector<Eigen::Index, Eigen::Dynamic> _block_column_offsets(
        number_of_column_blocks + 1);

    Eigen::Index constraint_index = 0;
    auto const &constrainttimes = constraint_interpolationpoints;
    Eigen::Index control_index = 1;
    auto const &controltimes = control_interpolationpoints;
    auto last_height = constraint_interpolationpoints.size();
    auto last_controltime = controltimes[0];
    _block_column_offsets[0] = 0;
    while (control_index != control_interpolationpoints.size()) {
      if (constraint_index == constraint_interpolationpoints.size()) {
        _block_column_offsets[control_index]
            = _block_column_offsets[control_index - 1] + last_height;
        last_height = 0;
        ++control_index;
      } else if (constrainttimes[constraint_index] > last_controltime) {
        _block_column_offsets[control_index]
            = _block_column_offsets[control_index - 1] + last_height;
        last_height = constraint_interpolationpoints.size() - constraint_index;

        if (control_index != control_interpolationpoints.size()) {
          last_controltime = controltimes[control_index];
        }
        ++control_index;
      } else {
        ++constraint_index;
      }
    }
    _block_column_offsets[control_index]
        = _block_column_offsets[control_index - 1] + last_height;

    return std::make_tuple(block_height, block_width, _block_column_offsets);
  }

  ConstraintJacobian_Base::ConstraintJacobian_Base(
      std::tuple<Eigen::Index, Eigen::Index, Eigen::VectorX<Eigen::Index>>
          height_width_offsets) :
      ConstraintJacobian_Base(
          std::get<0>(height_width_offsets), std::get<1>(height_width_offsets),
          std::get<2>(height_width_offsets)) {}

  ConstraintJacobian_Base::ConstraintJacobian_Base(
      Eigen::Index _block_height, Eigen::Index _block_width,
      Eigen::Vector<Eigen::Index, Eigen::Dynamic> _block_column_offsets) :
      block_height(_block_height),
      block_width(_block_width),
      block_column_offsets(std::move(_block_column_offsets)) {}

  Eigen::Index
  ConstraintJacobian_Base::get_outer_column_offset(Eigen::Index column) const {
    assert(column >= 0);
    assert(column < get_outer_width());

    return block_column_offsets[column];
  }
  Eigen::Index
  ConstraintJacobian_Base::get_inner_column_offset(Eigen::Index column) const {
    assert(column >= 0);
    assert(column < get_outer_width());
    return get_outer_column_offset(column) * get_block_size();
  }

  Eigen::Index ConstraintJacobian_Base::get_block_width() const {
    return block_width;
  }
  Eigen::Index ConstraintJacobian_Base::get_block_height() const {
    return block_height;
  }

  Eigen::Ref<Eigen::Vector<Eigen::Index, Eigen::Dynamic> const>
  ConstraintJacobian_Base::get_block_column_offsets() const {
    return block_column_offsets;
  }

  Eigen::Index ConstraintJacobian_Base::get_block_size() const {
    return get_block_height() * get_block_width();
  }
  Eigen::Index ConstraintJacobian_Base::get_outer_width() const {
    return block_column_offsets.size() - 1;
  }
  Eigen::Index ConstraintJacobian_Base::get_inner_width() const {
    return get_outer_width() * get_block_width();
  }
  Eigen::Index ConstraintJacobian_Base::get_outer_height() const {
    return block_column_offsets[1];
  }
  Eigen::Index ConstraintJacobian_Base::get_inner_height() const {
    return get_outer_height() * get_block_height();
  }
  Eigen::Index
  ConstraintJacobian_Base::get_outer_col_height(Eigen::Index column) const {
    assert(column >= 0);
    assert(column < get_outer_width());
    return block_column_offsets[column + 1] - block_column_offsets[column];
  }
  Eigen::Index
  ConstraintJacobian_Base::get_inner_col_height(Eigen::Index column) const {
    assert(column >= 0);
    assert(column < get_outer_width());
    return get_outer_col_height(column) * get_block_height();
  }

  Eigen::Index
  ConstraintJacobian_Base::get_outer_rowstart(Eigen::Index column) const {
    assert(column >= 0);
    assert(column < get_outer_width());
    return get_outer_height() - get_outer_col_height(column);
  }
  Eigen::Index
  ConstraintJacobian_Base::get_inner_rowstart(Eigen::Index column) const {
    return get_outer_rowstart(column) * get_block_height();
  }
  Eigen::Index
  ConstraintJacobian_Base::get_outer_colstart(Eigen::Index column) const {
    return column;
  }
  Eigen::Index
  ConstraintJacobian_Base::get_inner_colstart(Eigen::Index column) const {
    return get_outer_colstart(column) * get_block_width();
  }

  Eigen::Index
  ConstraintJacobian_Base::column_values_start(Eigen::Index column) const {
    return get_block_size() * block_column_offsets[column];
  }
  Eigen::Index
  ConstraintJacobian_Base::column_values_end(Eigen::Index column) const {
    return block_column_offsets[column + 1] * get_block_size();
  }

  void ConstraintJacobian_Base::setZero() { allvalues().setZero(); }
  void ConstraintJacobian_Base::setOnes() { allvalues().setOnes(); }

  Eigen::Ref<RowMat>
  ConstraintJacobian_Base::get_column_block(Eigen::Index column) {
    assert(0 <= column);
    assert(column < get_outer_width());
    return Eigen::Map<RowMat>(
        get_value_pointer() + get_inner_column_offset(column),
        get_inner_col_height(column), get_block_width());
  }
  Eigen::Ref<RowMat const>
  ConstraintJacobian_Base::get_column_block(Eigen::Index column) const {
    assert(0 <= column);
    assert(column < get_outer_width());

    return Eigen::Map<RowMat const>(
        get_value_pointer() + get_inner_column_offset(column),
        get_inner_col_height(column), get_block_width());
  }
  Eigen::Index ConstraintJacobian_Base::nonZeros() const {
    return block_column_offsets[block_column_offsets.size() - 1]
           * get_block_size();
  }

  void ConstraintJacobian_Base::supply_indices(
      Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Rowindices,
      Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Colindices) const {

    assert(Rowindices.size() == nonZeros());
    assert(Colindices.size() == nonZeros());
    Ipopt::Index index = 0;
    for (Eigen::Index j = 0; j != get_outer_width(); ++j) {
      auto columnoffset = get_inner_colstart(j);
      auto rowoffset = get_inner_rowstart(j);

      for (Eigen::Index innerrow = rowoffset; innerrow != get_inner_height();
           ++innerrow) {
        for (Eigen::Index innercol = columnoffset;
             innercol != columnoffset + get_block_width(); ++innercol) {
          assert(index < nonZeros());
          Rowindices[index] = static_cast<int>(innerrow);
          Colindices[index] = static_cast<int>(innercol);
          ++index;
        }
      }
    }
    assert(index == nonZeros());
  }

  Eigen::MatrixXd ConstraintJacobian_Base::whole_matrix() const {
    Eigen::MatrixXd result
        = Eigen::MatrixXd::Zero(get_inner_height(), get_inner_width());

    for (Eigen::Index column = 0; column != get_outer_width(); ++column) {
      result.block(
          get_inner_rowstart(column), get_inner_colstart(column),
          get_inner_col_height(column), get_block_width())
          = get_column_block(column);
    }
    return result;
  }

  Eigen::Ref<Eigen::VectorXd> ConstraintJacobian_Base::allvalues() {
    return Eigen::Map<Eigen::VectorXd>(get_value_pointer(), nonZeros());
  }
  Eigen::Ref<Eigen::VectorXd const>
  ConstraintJacobian_Base::get_allvalues() const {
    return Eigen::Map<Eigen::VectorXd const>(get_value_pointer(), nonZeros());
  }

  void ConstraintJacobian_Base::assignment_helper(
      ConstraintJacobian_Base const &other) {
    if (this->block_width != other.block_width) {
      gthrow(
          {"Can't assign a ConstraintJacobian to another one with different "
           "structure!"});
    }
    if (this->block_height != other.block_height) {
      gthrow(
          {"Can't assign a ConstraintJacobian to another one with different "
           "structure!"});
    }
    if (this->block_column_offsets.size()
        != other.block_column_offsets.size()) {
      gthrow(
          {"Can't assign a ConstraintJacobian to another one with different "
           "structure!"});
    }
    if (this->block_column_offsets != other.block_column_offsets) {
      gthrow(
          {"Can't assign a ConstraintJacobian to another one with different "
           "structure!"});
    }

    this->allvalues() = other.get_allvalues();
  }

  bool operator==(
      ConstraintJacobian_Base const &lhs, ConstraintJacobian_Base const &rhs) {
    if (lhs.get_block_height() != rhs.get_block_height()) {
      return false;
    }
    if (lhs.get_block_width() != rhs.get_block_width()) {
      return false;
    }
    if (lhs.get_block_column_offsets().size()
        != rhs.get_block_column_offsets().size()) {
      return false;
    }
    if (lhs.get_block_column_offsets() != rhs.get_block_column_offsets()) {
      return false;
    }
    // Size is equal because of the preceding if.
    if (lhs.get_allvalues() != rhs.get_allvalues()) {
      return false;
    }
    return true;
  }

  bool operator!=(
      ConstraintJacobian_Base const &lhs, ConstraintJacobian_Base const &rhs) {
    return !(lhs == rhs);
  }

  /////////////////////////////
  // MappedConstraintJacobian
  /////////////////////////////

  MappedConstraintJacobian::MappedConstraintJacobian(
      double *_values, Eigen::Index _number_of_entries,
      Eigen::Index number_of_constraints_per_step,
      Eigen::Index number_of_controls_per_step,
      Eigen::Ref<Eigen::VectorXd const> const &constraint_interpolationpoints,
      Eigen::Ref<Eigen::VectorXd const> const &control_interpolationpoints) :
      ConstraintJacobian_Base(make_data(
          number_of_constraints_per_step, number_of_controls_per_step,
          constraint_interpolationpoints, control_interpolationpoints)),
      values(_values),
      number_of_entries(_number_of_entries) {
    assert(nonZeros() == _number_of_entries);
  }

  void MappedConstraintJacobian::replace_storage(
      double *new_values,
      [[maybe_unused]] Eigen::Index length_must_be_equal_to_old_length) {
    assert(length_must_be_equal_to_old_length == number_of_entries);
    values = new_values;
  }

  MappedConstraintJacobian &
  MappedConstraintJacobian::operator=(MappedConstraintJacobian const &other) {
    assignment_helper(other);
    return *this;
  }
  MappedConstraintJacobian &
  MappedConstraintJacobian::operator=(ConstraintJacobian const &other) {
    assignment_helper(other);
    return *this;
  }

  double *MappedConstraintJacobian::get_value_pointer() {
    if (values == nullptr) {
      gthrow(
          {"You may not access the values in a MappedConstraintJacobian, that "
           "holds a NULL pointer!"});
    }
    return values;
  }
  double const *MappedConstraintJacobian::get_value_pointer() const {
    if (values == nullptr) {
      gthrow(
          {"You may not access the values in a MappedConstraintJacobian, that "
           "holds a NULL pointer!"});
    }
    return values;
  }

  /////////////////////////////////////////////////////////
  // ConstraintJacobian:
  /////////////////////////////////////////////////////////

  ConstraintJacobian::ConstraintJacobian(
      Eigen::Index number_of_constraints_per_step,
      Eigen::Index number_of_controls_per_step,
      Eigen::Ref<Eigen::VectorXd const> const &constraint_interpolationpoints,
      Eigen::Ref<Eigen::VectorXd const> const &control_interpolationpoints) :
      ConstraintJacobian_Base(make_data(
          number_of_constraints_per_step, number_of_controls_per_step,
          constraint_interpolationpoints, control_interpolationpoints)),
      storage(nonZeros()) {}

  ConstraintJacobian::ConstraintJacobian(ConstraintJacobian_Base const &other) :
      ConstraintJacobian_Base(other) {
    storage = other.get_allvalues();
  }

  ConstraintJacobian &
  ConstraintJacobian::operator=(MappedConstraintJacobian const &other) {
    assignment_helper(other);
    return *this;
  }
  ConstraintJacobian &
  ConstraintJacobian::operator=(ConstraintJacobian const &other) {
    assignment_helper(other);
    return *this;
  }

  double *ConstraintJacobian::get_value_pointer() { return storage.data(); }
  double const *ConstraintJacobian::get_value_pointer() const {
    return storage.data();
  }

} // namespace Optimization
