#include "ConstraintJacobian.hpp"
#include "Exception.hpp"
#include <Eigen/Dense>
#include <IpTypes.hpp>

namespace Optimization {

  std::tuple<Eigen::Index, Eigen::Index, Eigen::VectorX<Eigen::Index>>
  make_data(
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
    auto last_height = constraints.size();
    auto last_controltime = controltimes[0];
    _block_column_offsets[0] = 0;
    while (control_index != controls.size()) {
      if (constraint_index == constraints.size()) {
        _block_column_offsets[control_index]
            = _block_column_offsets[control_index - 1] + last_height;
        last_height = 0;
        ++control_index;
      } else if (constrainttimes[constraint_index] > last_controltime) {
        _block_column_offsets[control_index]
            = _block_column_offsets[control_index - 1] + last_height;
        last_height = constraints.size() - constraint_index;

        if (control_index != controls.size()) {
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

  Eigen::Index ConstraintJacobian_Base::get_outer_rowstart_jacobian(
      Eigen::Index column) const {
    assert(column >= 0);
    assert(column < get_outer_width());
    return get_outer_height() - get_outer_col_height(column);
  }
  Eigen::Index ConstraintJacobian_Base::get_inner_rowstart_jacobian(
      Eigen::Index column) const {
    return get_outer_rowstart_jacobian(column) * get_block_height();
  }
  Eigen::Index ConstraintJacobian_Base::get_outer_colstart_jacobian(
      Eigen::Index column) const {
    return column;
  }
  Eigen::Index ConstraintJacobian_Base::get_inner_colstart_jacobian(
      Eigen::Index column) const {
    return get_outer_colstart_jacobian(column) * get_block_width();
  }

  Eigen::Index
  ConstraintJacobian_Base::column_values_start(Eigen::Index column) const {
    return get_block_size() * block_column_offsets[column];
  }
  Eigen::Index
  ConstraintJacobian_Base::column_values_end(Eigen::Index column) const {
    return block_column_offsets[column + 1] * get_block_size();
  }

  void ConstraintJacobian_Base::setZero() { underlying_storage().setZero(); }

  Eigen::Ref<Eigen::MatrixXd>
  ConstraintJacobian_Base::get_column_block(Eigen::Index column) {
    return Eigen::Map<Eigen::MatrixXd>(
        get_value_pointer() + get_inner_column_offset(column),
        get_inner_col_height(column), get_block_width());
  }
  Eigen::Ref<Eigen::MatrixXd const>
  ConstraintJacobian_Base::get_column_block(Eigen::Index column) const {
    return Eigen::Map<Eigen::MatrixXd const>(
        get_value_pointer() + get_inner_column_offset(column),
        get_inner_col_height(column), get_block_width());
  }
  Eigen::Index ConstraintJacobian_Base::nonZeros() const {
    return block_column_offsets[block_column_offsets.size() - 1]
           * get_block_size();
  }

  void ConstraintJacobian_Base::supply_indices(
      Ipopt::Index *iRow, Ipopt::Index *jCol,
      Eigen::Index number_of_values) const {

    Ipopt::Index index = 0;
    for (Eigen::Index j = 0; j != get_outer_width(); ++j) {
      auto columnoffset = get_inner_colstart_jacobian(j);
      auto rowoffset = get_inner_rowstart_jacobian(j);

      for (Eigen::Index innercol = columnoffset;
           innercol != columnoffset + get_block_width(); ++innercol) {
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

  Eigen::MatrixXd ConstraintJacobian_Base::whole_matrix() const {
    Eigen::MatrixXd result
        = Eigen::MatrixXd::Zero(get_inner_height(), get_inner_width());

    for (Eigen::Index column = 0; column != get_outer_width(); ++column) {
      result.block(
          get_inner_rowstart_jacobian(column),
          get_inner_colstart_jacobian(column), get_inner_col_height(column),
          get_block_width())
          = get_column_block(column);
    }
    return result;
  }

  Eigen::Ref<Eigen::VectorXd> ConstraintJacobian_Base::underlying_storage() {
    return Eigen::Map<Eigen::VectorXd>(get_value_pointer(), nonZeros());
  }
  Eigen::Ref<Eigen::VectorXd const>
  ConstraintJacobian_Base::underlying_storage() const {
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

    this->underlying_storage() = other.underlying_storage();
  }

  /////////////////////////////
  // MappedConstraintJacobian
  /////////////////////////////

  MappedConstraintJacobian::MappedConstraintJacobian(
      double *_values, Eigen::Index _number_of_entries,
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls) :
      ConstraintJacobian_Base(make_data(constraints, controls)),
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

  double *MappedConstraintJacobian::get_value_pointer() { return values; }
  double const *MappedConstraintJacobian::get_value_pointer() const {
    return values;
  }

  /////////////////////////////////////////////////////////
  // ConstraintJacobian:
  /////////////////////////////////////////////////////////

  ConstraintJacobian::ConstraintJacobian(
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls) :
      ConstraintJacobian_Base(make_data(constraints, controls)),
      storage(nonZeros()) {}

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
