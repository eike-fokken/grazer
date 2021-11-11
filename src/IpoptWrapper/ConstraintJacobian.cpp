#include "ConstraintJacobian.hpp"

namespace Optimization {

  ConstraintJacobian::ConstraintJacobian(
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls, Ipopt::Number *values) {
    assert(false);
    // Check for off-by-one errors!
  }

  double &ConstraintJacobian::operator()(Eigen::Index row, Eigen::Index col) {
    assert(row >= 0);
    assert(col >= 0);
    assert(row < start_of_rows.size());
    auto rowstart = start_of_rows[row];
    auto element = rowstart + col;
    assert(element < start_of_rows[row + 1]);
    return values[element];
  }

  double
  ConstraintJacobian::operator()(Eigen::Index row, Eigen::Index col) const {
    assert(row >= 0);
    assert(col >= 0);
    assert(row < start_of_rows.size());
    auto rowstart = start_of_rows[row];
    auto element = rowstart + col;
    assert(element < start_of_rows[row + 1]);
    return values[element];
  }

  Eigen::Ref<Eigen::VectorXd> const ConstraintJacobian::row(Eigen::Index row) {
    assert(row >= 0);
    assert(row < start_of_rows.size());

    auto start = values + start_of_rows[row];
    auto end = values + start_of_rows[row + 1];
    Eigen::Map<Eigen::VectorXd> current_row(start, end - start);
    return current_row;
  }
  Eigen::Ref<Eigen::VectorXd const> const
  ConstraintJacobian::row(Eigen::Index row) const {
    assert(row >= 0);
    assert(row < start_of_rows.size());
    auto start = values + start_of_rows[row];
    auto end = values + start_of_rows[row + 1];
    Eigen::Map<Eigen::VectorXd> current_row(start, end - start);
    return current_row;
  }

  Eigen::Index ConstraintJacobian::size_of_row(Eigen::Index row) {
    assert(row >= 0);
    assert(row > start_of_rows.size() - 1);
    return start_of_rows[row + 1] - start_of_rows[row];
  }

} // namespace Optimization
