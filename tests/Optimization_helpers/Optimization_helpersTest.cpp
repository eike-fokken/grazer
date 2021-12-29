#include "Optimization_helpers.hpp"
#include <gtest/gtest.h>

TEST(compute_index_lambda, happy) {

  Eigen::VectorXd timepoints{{0, 1, 2, 3}};

  double start = timepoints[0];
  double end = timepoints[timepoints.size() - 1];
  double span = end - start;
  unsigned int N = 100;
  double delta = span / N;
  double computed_time = start - 1.0;
  for (unsigned int i = 0; i != N + 1; ++i) {
    double time = start + i * delta;
    auto [index, lambda] = Optimization::compute_index_lambda(timepoints, time);

    if (lambda == 0) {
      ADD_FAILURE();
    }
    if (lambda == 1.0) {
      computed_time = timepoints[index];
    } else {
      computed_time
          = (1 - lambda) * timepoints[index - 1] + lambda * timepoints[index];
    }
    EXPECT_DOUBLE_EQ(computed_time, time);
  }
}

TEST(compute_index_lambda_vector, happy) {
  Eigen::VectorXd coarse_timepoints{{0, 1, 2, 3}};
  double start = coarse_timepoints[0];
  double end = coarse_timepoints[coarse_timepoints.size() - 1];
  double span = end - start;
  Eigen::Index N = 100;
  Eigen::VectorXd fine_timepoints(N + 1);
  double delta = span / static_cast<double>(N);

  for (Eigen::Index i = 0; i != N + 1; ++i) {
    double time = start + static_cast<double>(i) * delta;
    fine_timepoints[i] = time;
  }
  auto index_lambda_pairs = Optimization::compute_index_lambd_vector(
      coarse_timepoints, fine_timepoints);

  double computed_time = start - 1.0;
  double lower_time = start - 1.0;
  double upper_time = start - 1.0;
  EXPECT_EQ(index_lambda_pairs.size(), fine_timepoints.size());
  for (Eigen::Index i = 0; i != N + 1; ++i) {
    double time = fine_timepoints[i];
    auto lambda = index_lambda_pairs[i].second;
    if (lambda == 0) {
      ADD_FAILURE();
    }

    upper_time = coarse_timepoints[index_lambda_pairs[i].first];
    if (lambda == 1.0) {
      computed_time = upper_time;
    } else {
      lower_time = coarse_timepoints[index_lambda_pairs[i].first - 1];
      computed_time = (1 - lambda) * lower_time + lambda * upper_time;
    }
    EXPECT_DOUBLE_EQ(computed_time, time);
  }
}
// TEST(compute_control_conversion, happy_path1) {

//   std::vector<double> fine_times{0.0, 2.0 / 3.0, 7.0 / 4.0, 2.0};
//   std::vector<double> coarse_times{0, 1, 2};

//   Eigen::Index inner_length = 3;
//   Aux::InterpolatingVector fine(fine_times, inner_length);
//   Aux::InterpolatingVector fine2(fine_times, inner_length);
//   Aux::InterpolatingVector coarse(coarse_times, inner_length);
//   Aux::InterpolatingVector coarse2(coarse_times, inner_length);
//   Eigen::VectorXd values{{1, 3, -2, 2, 6, 7, 9, 3, 1}};

//   coarse.set_values_in_bulk(values);

//   double h = 10;
//   for (Eigen::Index direction = 0; direction != values.size(); ++direction)
//   {
//     Eigen::VectorXd values2 = values;
//     values2[direction] += h;
//     coarse2.set_values_in_bulk(values2);

//     for (Eigen::Index i = 0; i != fine.size(); ++i) {
//       fine.mut_timestep(i) = coarse(fine_times[static_cast<size_t>(i)]);
//       fine2.mut_timestep(i) = coarse2(fine_times[static_cast<size_t>(i)]);
//     }

//     Eigen::SparseMatrix<double> derivative
//         = Optimization::compute_control_conversion(fine, coarse);

//     Eigen::VectorXd res
//         = fine2.get_allvalues() - fine.get_allvalues()
//           - derivative * (coarse2.get_allvalues() -
//           coarse.get_allvalues());

//     for (Eigen::Index i = 0; i != res.size(); ++i) {
//       EXPECT_NEAR(res[i], 0.0, 1e-14);
//     }
//   }
// }

// TEST(compute_control_conversion, happy_path2) {

//   std::vector<double> fine_times{0.1, 2.0 / 3.0, 7.0 / 4.0, 1.8};
//   std::vector<double> coarse_times{0, 1, 2};

//   Eigen::Index inner_length = 3;
//   Aux::InterpolatingVector fine(fine_times, inner_length);
//   Aux::InterpolatingVector fine2(fine_times, inner_length);
//   Aux::InterpolatingVector coarse(coarse_times, inner_length);
//   Aux::InterpolatingVector coarse2(coarse_times, inner_length);
//   Eigen::VectorXd values{{1, 3, -2, 2, 6, 7, 9, 3, 1}};

//   coarse.set_values_in_bulk(values);

//   double h = 1e-3;

//   for (Eigen::Index direction = 0; direction != values.size(); ++direction)
//   {
//     Eigen::VectorXd values2 = values;
//     values2[direction] += h;
//     coarse2.set_values_in_bulk(values2);

//     for (Eigen::Index i = 0; i != fine.size(); ++i) {
//       fine.mut_timestep(i) = coarse(fine_times[static_cast<size_t>(i)]);
//       fine2.mut_timestep(i) = coarse2(fine_times[static_cast<size_t>(i)]);
//     }

//     Eigen::SparseMatrix<double> derivative
//         = Optimization::compute_control_conversion(fine, coarse);

//     Eigen::VectorXd res
//         = fine2.get_allvalues() - fine.get_allvalues()
//           - derivative * (coarse2.get_allvalues() -
//           coarse.get_allvalues());

//     for (Eigen::Index i = 0; i != res.size(); ++i) {
//       EXPECT_NEAR(res[i], 0.0, 1e-14);
//     }
//   }
// }

// TEST(compute_control_conversion, happy_path3) {

//   std::vector<double> fine_times{0, 1, 2};
//   std::vector<double> coarse_times{0, 1, 2};

//   Eigen::Index inner_length = 3;
//   Aux::InterpolatingVector fine(fine_times, inner_length);
//   Aux::InterpolatingVector fine2(fine_times, inner_length);
//   Aux::InterpolatingVector coarse(coarse_times, inner_length);
//   Aux::InterpolatingVector coarse2(coarse_times, inner_length);
//   Eigen::VectorXd values{{1, 3, -2, 2, 6, 7, 9, 3, 1}};

//   coarse.set_values_in_bulk(values);

//   double h = 1e-3;

//   for (Eigen::Index direction = 0; direction != values.size(); ++direction)
//   {
//     Eigen::VectorXd values2 = values;
//     values2[direction] += h;
//     coarse2.set_values_in_bulk(values2);

//     for (Eigen::Index i = 0; i != fine.size(); ++i) {
//       fine.mut_timestep(i) = coarse(fine_times[static_cast<size_t>(i)]);
//       fine2.mut_timestep(i) = coarse2(fine_times[static_cast<size_t>(i)]);
//     }

//     Eigen::SparseMatrix<double> derivative
//         = Optimization::compute_control_conversion(fine, coarse);

//     Eigen::VectorXd res
//         = fine2.get_allvalues() - fine.get_allvalues()
//           - derivative * (coarse2.get_allvalues() -
//           coarse.get_allvalues());

//     for (Eigen::Index i = 0; i != res.size(); ++i) {
//       EXPECT_NEAR(res[i], 0.0, 1e-14);
//     }
//   }
// }
