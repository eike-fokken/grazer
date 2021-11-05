#include "Optimization_helpers.hpp"
#include "InterpolatingVector.hpp"
#include <Eigen/src/SparseCore/SparseMatrix.h>
#include <cstddef>
#include <gtest/gtest.h>

TEST(compute_control_conversion, happy_path1) {

  std::vector<double> fine_times{0.0, 2.0 / 3.0, 7.0 / 4.0, 2.0};
  std::vector<double> coarse_times{0, 1, 2};

  Eigen::Index inner_length = 3;
  Aux::InterpolatingVector fine(fine_times, inner_length);
  Aux::InterpolatingVector fine2(fine_times, inner_length);
  Aux::InterpolatingVector coarse(coarse_times, inner_length);
  Aux::InterpolatingVector coarse2(coarse_times, inner_length);
  Eigen::VectorXd values{{1, 3, -2, 2, 6, 7, 9, 3, 1}};

  coarse.set_values_in_bulk(values);

  double h = 10;
  Eigen::VectorXd values2 = values;
  values2[values2.size() - 1] += h;
  Eigen::VectorXd difference{{0, 0, h}};

  coarse2.set_values_in_bulk(values2);

  for (Eigen::Index i = 0; i != fine.size(); ++i) {
    fine.mut_timestep(i) = coarse(fine_times[static_cast<size_t>(i)]);
    fine2.mut_timestep(i) = coarse2(fine_times[static_cast<size_t>(i)]);
  }

  Eigen::SparseMatrix<double> derivative
      = Optimization::compute_control_conversion(fine, coarse);

  Eigen::VectorXd res
      = fine2.get_allvalues() - fine.get_allvalues()
        - derivative * (coarse2.get_allvalues() - coarse.get_allvalues());

  for (Eigen::Index i = 0; i != res.size(); ++i) {
    EXPECT_NEAR(res[i], 0.0, 1e-14);
  }
}

TEST(compute_control_conversion, happy_path2) {

  std::vector<double> fine_times{0.1, 2.0 / 3.0, 7.0 / 4.0, 1.8};
  std::vector<double> coarse_times{0, 1, 2};

  Eigen::Index inner_length = 3;
  Aux::InterpolatingVector fine(fine_times, inner_length);
  Aux::InterpolatingVector fine2(fine_times, inner_length);
  Aux::InterpolatingVector coarse(coarse_times, inner_length);
  Aux::InterpolatingVector coarse2(coarse_times, inner_length);
  Eigen::VectorXd values{{1, 3, -2, 2, 6, 7, 9, 3, 1}};

  coarse.set_values_in_bulk(values);

  double h = 1e-6;
  Eigen::VectorXd values2 = values;
  values2[values2.size() - 1] += h;
  Eigen::VectorXd difference{{0, 0, h}};

  coarse2.set_values_in_bulk(values2);

  for (Eigen::Index i = 0; i != fine.size(); ++i) {
    fine.mut_timestep(i) = coarse(fine_times[static_cast<size_t>(i)]);
    fine2.mut_timestep(i) = coarse2(fine_times[static_cast<size_t>(i)]);
  }

  Eigen::SparseMatrix<double> derivative
      = Optimization::compute_control_conversion(fine, coarse);

  Eigen::VectorXd res
      = fine2.get_allvalues() - fine.get_allvalues()
        - derivative * (coarse2.get_allvalues() - coarse.get_allvalues());

  for (Eigen::Index i = 0; i != res.size(); ++i) {
    EXPECT_NEAR(res[i], 0.0, 1e-14);
  }
}
