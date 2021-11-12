#include "ConstraintJacobian.hpp"

namespace Optimization {

  ConstraintJacobian::ConstraintJacobian(
      Aux::InterpolatingVector_Base const &constraints,
      Aux::InterpolatingVector_Base const &controls) {
    assert(false);
    // Check for off-by-one errors!
  }

  double &ConstraintJacobian::CoeffRef(
      Ipopt::Number *values, [[maybe_unused]] Ipopt::Index values_end,
      Eigen::Index row, Eigen::Index col) {
    assert(values_end == nnz);
    assert(row >= 0);
    assert(col >= 0);
    assert(row < start_of_rows.size() - 1);
    auto rowstart = start_of_rows[row];
    auto element = rowstart + col;
    assert(element < start_of_rows[row + 1]);
    return values[element];
  }

  double ConstraintJacobian::Coeff(
      Ipopt::Number *values, [[maybe_unused]] Ipopt::Index values_end,
      Eigen::Index row, Eigen::Index col) const {
    assert(values_end == nnz);
    assert(row >= 0);
    assert(col >= 0);
    assert(row < start_of_rows.size() - 1);
    auto rowstart = start_of_rows[row];
    auto element = rowstart + col;
    assert(element < start_of_rows[row + 1]);
    return values[element];
  }

  Eigen::Map<Eigen::VectorXd> const ConstraintJacobian::row(
      Ipopt::Number *values, [[maybe_unused]] Ipopt::Index values_end,
      Eigen::Index row) {
    assert(values_end == nnz);
    assert(row >= 0);
    assert(row < start_of_rows.size() - 1);

    auto start = values + start_of_rows[row];
    auto end = values + start_of_rows[row + 1];
    Eigen::Map<Eigen::VectorXd> current_row(start, end - start);
    return current_row;
  }
  Eigen::Map<Eigen::VectorXd const> const ConstraintJacobian::row(
      Ipopt::Number *values, [[maybe_unused]] Ipopt::Index values_end,
      Eigen::Index row) const {
    assert(values_end == nnz);
    assert(row >= 0);
    assert(row < start_of_rows.size() - 1);
    auto start = values + start_of_rows[row];
    auto end = values + start_of_rows[row + 1];
    Eigen::Map<Eigen::VectorXd const> current_row(start, end - start);
    return current_row;
  }

  Eigen::Index ConstraintJacobian::size_of_row(Eigen::Index row) {
    assert(row >= 0);
    assert(row < start_of_rows.size() - 1);
    return start_of_rows[row + 1] - start_of_rows[row];
  }

  Eigen::Index ConstraintJacobian::rows() const {
    return start_of_rows.size() - 1;
  }
  Eigen::Index ConstraintJacobian::cols() const { return number_of_columns; }

  Eigen::Index ConstraintJacobian::nonZeros() const { return nnz; }

} // namespace Optimization
