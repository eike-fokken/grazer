#include "ConstraintJacobian.hpp"
#include "InterpolatingVector.hpp"
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
    Eigen::MatrixXd a = jacobian.get_nnz_column_block(j);
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

  Eigen::Index counter = 0;
  for (Eigen::Index column = 0; column != jacobian.get_outer_width();
       ++column) {
    // std::cout << "column: " << column << std::endl;
    // std::cout << "rows: " << jacobian.get_nnz_column_block(column).rows()
    //           << std::endl;
    // std::cout << "cols: " << jacobian.get_nnz_column_block(column).cols()
    //           << std::endl;
    // counter += jacobian.get_nnz_column_block(column).rows();
    jacobian.get_nnz_column_block(column).setOnes();
  }

  // counter *= jacobian.get_block_width();
  // std::cout << "counter: " << counter << std::endl;
  // std::cout << "nonZeros: " << jacobian.nonZeros() << std::endl;
  std::cout << jacobian.whole_matrix() << std::endl;
}
// TEST(ConstraintJacobian, coeff) {
//   // std::vector<double> constraints_times{0.0, 0.5, 1.0, 2.0};
//   // std::vector<double> controls_times{0, 1, 2};

//   // constexpr Eigen::Index constraints_inner_length = 2;
//   // constexpr Eigen::Index controls_inner_length = 3;
//   // Aux::InterpolatingVector constraints(
//   //     constraints_times, constraints_inner_length);
//   // Aux::InterpolatingVector controls(controls_times,
//   controls_inner_length);

//   // auto jacobian
//   //     = Optimization::ConstraintJacobian::make_instance(constraints,
//   //     controls);
//   // std::vector<double> value_vector(
//   //     static_cast<std::size_t>(jacobian.nonZeros()));

//   // {
//   //   double i = 0;
//   //   for (auto &value : value_vector) {
//   //     value = i;
//   //     ++i;
//   //   }
//   // }
//   // auto values = value_vector.data();
//   // auto values_end = static_cast<Ipopt::Index>(value_vector.size());

//   // for (int i = 0; i != jacobian.rows(); ++i) {
//   //   for (int j = 0; j != jacobian.cols(); ++j) {
//   //     std::cout << std::setw(2) << jacobian.Coeff(values, values_end, i,
//   j)
//   //               << " ";
//   //   }
//   //   std::cout << "\n";
//   // }

//   // std::cout << "number of non-Zeros: " << jacobian.nonZeros() <<
//   std::endl;
//   // EXPECT_EQ(jacobian.nonZeros(), 48);
// }

// TEST(ConstraintJacobian, row) {
//   // std::vector<double> constraints_times{0.0, 0.5, 1.0, 2.0};
//   // std::vector<double> controls_times{0, 1, 2};

//   // constexpr Eigen::Index constraints_inner_length = 2;
//   // constexpr Eigen::Index controls_inner_length = 3;
//   // Aux::InterpolatingVector constraints(
//   //     constraints_times, constraints_inner_length);
//   // Aux::InterpolatingVector controls(controls_times,
//   controls_inner_length);

//   // auto jacobian
//   //     = Optimization::ConstraintJacobian::make_instance(constraints,
//   //     controls);
//   // std::vector<double> value_vector(
//   //     static_cast<std::size_t>(jacobian.nonZeros()));

//   // {
//   //   double i = 0;
//   //   for (auto &value : value_vector) {
//   //     value = 0;
//   //     ++i;
//   //   }
//   // }
//   // auto values = value_vector.data();
//   // auto values_end = static_cast<Ipopt::Index>(value_vector.size());

//   // for (int row = 0; row != jacobian.rows(); ++row) {
//   //   Eigen::VectorXd ones(jacobian.size_of_row(row));
//   //   ones.setOnes();
//   //   jacobian.row(values, values_end, row) = ones;
//   // }

//   // for (int row = 0; row != jacobian.rows(); ++row) {
//   //   for (int j = 0; j != jacobian.cols(); ++j) {
//   //     std::cout << std::setw(2) << jacobian.Coeff(values, values_end,
//   row, j)
//   //               << " ";
//   //   }
//   //   std::cout << "\n";
//   // }

//   // std::cout << "number of non-Zeros: " << jacobian.nonZeros() <<
//   std::endl;
//   // EXPECT_EQ(jacobian.nonZeros(), 48);
// }
