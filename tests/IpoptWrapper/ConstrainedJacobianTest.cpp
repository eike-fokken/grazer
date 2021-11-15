#include "ConstraintJacobian.hpp"
#include "InterpolatingVector.hpp"
#include <Eigen/src/Core/util/Meta.h>
#include <cstddef>
#include <gtest/gtest.h>
#include <sys/stat.h>

TEST(ConstraintJacobian, construction) {

  std::vector<double> constraints_times{0.0, 0.5, 1.0, 2.0};
  std::vector<double> controls_times{0, 1, 2};

  constexpr Eigen::Index constraints_inner_length = 2;
  constexpr Eigen::Index controls_inner_length = 3;
  Aux::InterpolatingVector constraints(
      constraints_times, constraints_inner_length);
  Aux::InterpolatingVector controls(controls_times, controls_inner_length);

  auto test
      = Optimization::ConstraintJacobian::make_instance(constraints, controls);

  EXPECT_EQ(test.cols(), controls.get_total_number_of_values());
  EXPECT_EQ(test.rows(), constraints.get_total_number_of_values());

  auto start_of_rows = test.get_start_of_rows();
  Eigen::Index curr = 0;
  EXPECT_EQ(start_of_rows[0], 0);
  for (Eigen::Index i = 0; i != start_of_rows.size() - 1; ++i) {
    curr += test.size_of_row(i);
    EXPECT_EQ(start_of_rows[i + 1], curr);
  }
}
