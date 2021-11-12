#include "ConstraintJacobian.hpp"
#include "InterpolatingVector.hpp"
#include <Eigen/src/Core/util/Meta.h>
#include <cstddef>
#include <gtest/gtest.h>

TEST(ConstraintJacobian, construction) {

  std::vector<double> constraints_times{0.0, 1.0};
  std::vector<double> controls_times{0, 1};

  constexpr Eigen::Index constraints_inner_length = 2;
  constexpr Eigen::Index controls_inner_length = 3;
  Aux::InterpolatingVector constraints(
      constraints_times, constraints_inner_length);
  Aux::InterpolatingVector controls(controls_times, controls_inner_length);

  auto test
      = Optimization::ConstraintJacobian::make_instance(constraints, controls);

  // This must go into a constructor
  double *values = new double[static_cast<std::size_t>(test.nonZeros())];

  for (Eigen::Index i = 0; i != test.rows(); ++i) {
    std::cout << test.size_of_row(i) << std::endl;
  }

  std::cout << "non-zeros: " << test.nonZeros() << std::endl;

  std::cout << "start_of_rows:" << std::endl;
  auto a = test.get_start_of_rows();
  Eigen::Index curr = 0;
  std::cout << a[0] << " ";
  for (Eigen::Index i = 0; i != a.size() - 1; ++i) {
    curr += test.size_of_row(i);
    std::cout << a[i + 1] - curr << " ";
  }
  std::cout << std::endl;

  // This must go into a destructor:
  delete[] values;
}
