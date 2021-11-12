#pragma once
#include "InterpolatingVector.hpp"
#include <Eigen/Dense>
#include <IpTypes.hpp>
#include <vector>

namespace Optimization {

  class ConstraintJacobian {
  public:
    ConstraintJacobian(
        Aux::InterpolatingVector_Base const &constraints,
        Aux::InterpolatingVector_Base const &controls, Ipopt::Number *values);

    double &operator()(Eigen::Index row, Eigen::Index col);

    double operator()(Eigen::Index row, Eigen::Index col) const;

    Eigen::Ref<Eigen::VectorXd> const row(Eigen::Index row_index);
    Eigen::Ref<Eigen::VectorXd const> const row(Eigen::Index row_index) const;

    Eigen::Index size_of_row(Eigen::Index row);

  private:
    /** @brief For each row this vector holds the starting index
     */
    Eigen::Vector<Eigen::Index, Eigen::Dynamic> start_of_rows;

    /** Non-owning copy of the values array of the jacobian matrix
     */
    Ipopt::Number *values;

    /** @brief length of the values array.
     */
    Ipopt::Index nnz;
  };

} // namespace Optimization
