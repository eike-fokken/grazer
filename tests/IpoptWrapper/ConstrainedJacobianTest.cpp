#include "ConstraintJacobian.hpp"
#include "InterpolatingVector.hpp"
#include <Eigen/src/Core/util/Meta.h>
#include <gtest/gtest.h>

#include <iomanip>

TEST(ConstraintJacobian, construction) {

  std::vector<double> constraints_times{0.0, 0.5, 1.0, 2.0};
  std::vector<double> controls_times{0, 1, 2};

  constexpr Eigen::Index constraints_inner_length = 2;
  constexpr Eigen::Index controls_inner_length = 3;
  Aux::InterpolatingVector constraints(
      constraints_times, constraints_inner_length);
  Aux::InterpolatingVector controls(controls_times, controls_inner_length);

  auto jacobian
      = Optimization::ConstraintJacobian::make_instance(constraints, controls);

  EXPECT_EQ(jacobian.cols(), controls.get_total_number_of_values());
  EXPECT_EQ(jacobian.rows(), constraints.get_total_number_of_values());

  auto start_of_rows = jacobian.get_start_of_rows();
  Eigen::Index curr = 0;
  EXPECT_EQ(start_of_rows[0], 0);
  for (Eigen::Index i = 0; i != start_of_rows.size() - 1; ++i) {
    curr += jacobian.size_of_row(i);
    EXPECT_EQ(start_of_rows[i + 1], curr);
  }
}

TEST(ConstraintJacobian, coeff) {
  std::vector<double> constraints_times{0.0, 0.5, 1.0, 2.0};
  std::vector<double> controls_times{0, 1, 2};

  constexpr Eigen::Index constraints_inner_length = 2;
  constexpr Eigen::Index controls_inner_length = 3;
  Aux::InterpolatingVector constraints(
      constraints_times, constraints_inner_length);
  Aux::InterpolatingVector controls(controls_times, controls_inner_length);

  auto jacobian
      = Optimization::ConstraintJacobian::make_instance(constraints, controls);
  std::vector<double> value_vector(
      static_cast<std::size_t>(jacobian.nonZeros()));

  {
    double i = 0;
    for (auto &value : value_vector) {
      value = i;
      ++i;
    }
  }
  auto values = value_vector.data();
  auto values_end = static_cast<Ipopt::Index>(value_vector.size());

  for (int i = 0; i != jacobian.rows(); ++i) {
    for (int j = 0; j != jacobian.cols(); ++j) {
      std::cout << std::setw(2) << jacobian.Coeff(values, values_end, i, j)
                << " ";
    }
    std::cout << "\n";
  }

  std::cout << "number of non-Zeros: " << jacobian.nonZeros() << std::endl;
  EXPECT_EQ(jacobian.nonZeros(), 48);
}

TEST(ConstraintJacobian, row) {
  std::vector<double> constraints_times{0.0, 0.5, 1.0, 2.0};
  std::vector<double> controls_times{0, 1, 2};

  constexpr Eigen::Index constraints_inner_length = 2;
  constexpr Eigen::Index controls_inner_length = 3;
  Aux::InterpolatingVector constraints(
      constraints_times, constraints_inner_length);
  Aux::InterpolatingVector controls(controls_times, controls_inner_length);

  auto jacobian
      = Optimization::ConstraintJacobian::make_instance(constraints, controls);
  std::vector<double> value_vector(
      static_cast<std::size_t>(jacobian.nonZeros()));

  {
    double i = 0;
    for (auto &value : value_vector) {
      value = 0;
      ++i;
    }
  }
  auto values = value_vector.data();
  auto values_end = static_cast<Ipopt::Index>(value_vector.size());

  for (int row = 0; row != jacobian.rows(); ++row) {
    Eigen::VectorXd ones(jacobian.size_of_row(row));
    ones.setOnes();
    jacobian.row(values, values_end, row) = ones;
  }

  for (int row = 0; row != jacobian.rows(); ++row) {
    for (int j = 0; j != jacobian.cols(); ++j) {
      std::cout << std::setw(2) << jacobian.Coeff(values, values_end, row, j)
                << " ";
    }
    std::cout << "\n";
  }

  std::cout << "number of non-Zeros: " << jacobian.nonZeros() << std::endl;
  EXPECT_EQ(jacobian.nonZeros(), 48);
}
