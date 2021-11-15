#include "ConstraintJacobian.hpp"
#include "Exception.hpp"

namespace Optimization {

  ConstraintJacobian::ConstraintJacobian(
      Eigen::Vector<Eigen::Index, Eigen::Dynamic> _start_indices_of_rows,
      Eigen::Index _number_of_columns) :
      start_of_rows(std::move(_start_indices_of_rows)),
      number_of_columns(_number_of_columns) {}

  double &ConstraintJacobian::CoeffRef(
      Ipopt::Number *values, [[maybe_unused]] Ipopt::Index values_end,
      Eigen::Index row, Eigen::Index col) {
    assert(values_end == nonZeros());
    assert(row >= 0);
    assert(row < rows());
    assert(col >= 0);
    assert(col < cols());

    auto rowstart = start_of_rows[row];
    auto element = rowstart + col;

    if (element < start_of_rows[row + 1]) {
      return values[element];
    } else {
      gthrow(
          {"You try to get a mutable reference to this element of the "
           "ConstraintJacobian: It is not among "
           "the structurally non-zero elements!"});
    }
  }

  double ConstraintJacobian::Coeff(
      Ipopt::Number *values, [[maybe_unused]] Ipopt::Index values_end,
      Eigen::Index row, Eigen::Index col) const {
    assert(values_end == nonZeros());
    assert(row >= 0);
    assert(row < rows());
    assert(col >= 0);
    assert(col < cols());

    auto rowstart = start_of_rows[row];
    auto element = rowstart + col;
    if (element < start_of_rows[row + 1]) {
      return values[element];
    } else {
      return 0;
    }
  }

  Eigen::Map<Eigen::VectorXd> const ConstraintJacobian::row(
      Ipopt::Number *values, [[maybe_unused]] Ipopt::Index values_end,
      Eigen::Index row) {
    assert(values_end == nonZeros());
    assert(row >= 0);
    assert(row < rows());

    auto start = values + start_of_rows[row];
    auto end = values + start_of_rows[row + 1];
    Eigen::Map<Eigen::VectorXd> current_row(start, end - start);
    return current_row;
  }
  Eigen::Map<Eigen::VectorXd const> const ConstraintJacobian::row(
      Ipopt::Number *values, [[maybe_unused]] Ipopt::Index values_end,
      Eigen::Index row) const {
    assert(values_end == nonZeros());
    assert(row >= 0);
    assert(row < rows());
    auto start = values + start_of_rows[row];
    auto end = values + start_of_rows[row + 1];
    Eigen::Map<Eigen::VectorXd const> current_row(start, end - start);
    return current_row;
  }

  Eigen::Index ConstraintJacobian::size_of_row(Eigen::Index row) {
    assert(row >= 0);
    assert(row < rows());
    return start_of_rows[row + 1] - start_of_rows[row];
  }

  Eigen::Index ConstraintJacobian::rows() const {
    return start_of_rows.size() - 1;
  }
  Eigen::Index ConstraintJacobian::cols() const { return number_of_columns; }

  Eigen::Index ConstraintJacobian::nonZeros() const {
    return start_of_rows[start_of_rows.size() - 1];
  }

  ConstraintJacobian ConstraintJacobian::make_instance(
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls) {

    auto number_of_rows = constraints.get_total_number_of_values();
    auto number_of_columns = controls.get_total_number_of_values();
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> start_indices_of_rows(
        number_of_rows + 1);
    // first row starts at 0
    auto it = start_indices_of_rows.begin();
    *it = 0;
    ++it;

    auto constraints_times = constraints.get_interpolation_points();
    auto controls_times = controls.get_interpolation_points();
    auto ntimesteps = constraints_times.size();

    auto number_controls_per_timestep = controls.get_inner_length();
    auto number_constraints_per_timestep = constraints.get_inner_length();

    for (size_t index = 0; index != ntimesteps; ++index) {
      auto t_j_interator = std::lower_bound(
          controls_times.begin(), controls_times.end(),
          constraints_times[index]);
      auto j = (t_j_interator - controls_times.begin());

      for (auto row_index
           = static_cast<Eigen::Index>(index) * number_constraints_per_timestep;
           row_index
           != static_cast<Eigen::Index>(index + 1)
                  * number_constraints_per_timestep;
           ++row_index) {
        *it = *std::prev(it) + (j + 1) * number_controls_per_timestep;
        ++it;
      }
    }

    return ConstraintJacobian(
        std::move(start_indices_of_rows), number_of_columns);
  }

} // namespace Optimization
