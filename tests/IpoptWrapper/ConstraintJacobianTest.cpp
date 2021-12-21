#include "ConstraintJacobian.hpp"
#include "InterpolatingVector.hpp"
#include <Eigen/src/Core/util/Constants.h>
#include <gtest/gtest.h>

#include <iomanip>

TEST(ConstraintJacobian, construction1) {

  Eigen::Vector<double, Eigen::Dynamic> constraints_times{
      {0.0, 0.5, 0.6, 1.0, 2.0}};
  Eigen::Vector<double, Eigen::Dynamic> controls_times{{-1, 0, 1, 2, 3}};

  Eigen::Index constraints_inner_length = 2;
  Eigen::Index controls_inner_length = 3;
  Aux::InterpolatingVector constraints(
      constraints_times, constraints_inner_length);
  Aux::InterpolatingVector controls(controls_times, controls_inner_length);

  auto jacobian = Optimization::ConstraintJacobian(constraints, controls);

  EXPECT_EQ(jacobian.get_block_height(), constraints.get_inner_length());
  EXPECT_EQ(jacobian.get_block_width(), controls.get_inner_length());
  EXPECT_EQ(
      jacobian.get_block_size(),
      jacobian.get_block_height() * jacobian.get_block_width());
  EXPECT_EQ(jacobian.get_outer_height(), constraints.size());
  EXPECT_EQ(jacobian.get_outer_width(), controls.size());

  Eigen::Index expected_nonzeros = 0;
  for (Eigen::Index column = 0; column != jacobian.get_outer_width();
       ++column) {
    auto current_block = jacobian.get_column_block(column);
    expected_nonzeros += current_block.rows() * current_block.cols();
  }
  EXPECT_EQ(jacobian.nonZeros(), expected_nonzeros);
}

TEST(ConstraintJacobian, column_height) {

  Eigen::Vector<double, Eigen::Dynamic> constraints_times{
      {0.0, 0.5, 0.6, 1.0, 2.0}};
  Eigen::Vector<double, Eigen::Dynamic> controls_times{{-1, 0, 1, 2, 3}};

  Eigen::Index constraints_inner_length = 2;
  Eigen::Index controls_inner_length = 3;
  Aux::InterpolatingVector constraints(
      constraints_times, constraints_inner_length);
  Aux::InterpolatingVector controls(controls_times, controls_inner_length);

  auto jacobian = Optimization::ConstraintJacobian(constraints, controls);

  for (Eigen::Index j = 0; j != jacobian.get_outer_width(); ++j) {
    Eigen::MatrixXd a = jacobian.get_column_block(j);
    EXPECT_EQ(a.rows(), jacobian.get_inner_col_height(j));
    EXPECT_EQ(
        a.rows(),
        jacobian.get_outer_col_height(j) * jacobian.get_block_height());
  }
}

TEST(ConstraintJacobian, setmatrix) {
  Eigen::Vector<double, Eigen::Dynamic> constraints_times{{0.0, 0.5, 1.0, 2.0}};
  Eigen::Vector<double, Eigen::Dynamic> controls_times{{0, 1, 2}};

  Eigen::Index constraints_inner_length = 2;
  Eigen::Index controls_inner_length = 3;
  Aux::InterpolatingVector constraints(
      constraints_times, constraints_inner_length);
  Aux::InterpolatingVector controls(controls_times, controls_inner_length);

  auto jacobian = Optimization::ConstraintJacobian(constraints, controls);

  jacobian.setZero();

  for (Eigen::Index column = 0; column != jacobian.get_outer_width();
       ++column) {
    jacobian.get_column_block(column).setOnes();
  }

  auto total_height = constraints.get_total_number_of_values();
  auto total_width = controls.get_total_number_of_values();
  Eigen::MatrixXd expected_matrix(total_height, total_width);
  expected_matrix.setZero();
  Eigen::Vector<Eigen::Index, Eigen::Dynamic> rowstarts{{0, 2, 6}};
  Eigen::Vector<Eigen::Index, Eigen::Dynamic> colstarts{{0, 3, 6}};
  auto blockwidth = controls.get_inner_length();
  for (Eigen::Index column = 0; column != jacobian.get_outer_width();
       ++column) {
    auto current_height = total_height - rowstarts[column];
    expected_matrix
        .block(rowstarts[column], colstarts[column], current_height, blockwidth)
        .setOnes();
  }
  EXPECT_EQ(jacobian.whole_matrix(), expected_matrix);
}

TEST(ConstraintJacobian, assignment_happy) {
  Eigen::Vector<double, Eigen::Dynamic> constraints_times{{0.0, 0.5, 1.0, 2.0}};
  Eigen::Vector<double, Eigen::Dynamic> controls_times{{0, 1, 2}};

  Eigen::Index constraints_inner_length = 2;
  Eigen::Index controls_inner_length = 3;
  Aux::InterpolatingVector constraints(
      constraints_times, constraints_inner_length);
  Aux::InterpolatingVector controls(controls_times, controls_inner_length);

  auto jacobian1 = Optimization::ConstraintJacobian(constraints, controls);
  auto jacobian2 = Optimization::ConstraintJacobian(constraints, controls);

  jacobian1.setZero();
  jacobian2.setZero();

  for (Eigen::Index column = 0; column != jacobian1.get_outer_width();
       ++column) {
    jacobian1.get_column_block(column).setOnes();
  }

  jacobian2 = jacobian1;

  EXPECT_EQ(jacobian1.whole_matrix(), jacobian2.whole_matrix());
}
