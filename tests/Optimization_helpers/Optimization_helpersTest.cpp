#include "Optimization_helpers.hpp"
#include "InterpolatingVector.hpp"
#include <Eigen/src/Core/util/Constants.h>
#include <Eigen/src/SparseCore/SparseMatrix.h>
#include <algorithm>
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
  for (Eigen::Index direction = 0; direction != values.size(); ++direction) {
    Eigen::VectorXd values2 = values;
    values2[direction] += h;
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

  double h = 1e-3;

  for (Eigen::Index direction = 0; direction != values.size(); ++direction) {
    Eigen::VectorXd values2 = values;
    values2[direction] += h;
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
}

TEST(compute_control_conversion, happy_path3) {

  std::vector<double> fine_times{0, 1, 2};
  std::vector<double> coarse_times{0, 1, 2};

  Eigen::Index inner_length = 3;
  Aux::InterpolatingVector fine(fine_times, inner_length);
  Aux::InterpolatingVector fine2(fine_times, inner_length);
  Aux::InterpolatingVector coarse(coarse_times, inner_length);
  Aux::InterpolatingVector coarse2(coarse_times, inner_length);
  Eigen::VectorXd values{{1, 3, -2, 2, 6, 7, 9, 3, 1}};

  coarse.set_values_in_bulk(values);

  double h = 1e-3;

  for (Eigen::Index direction = 0; direction != values.size(); ++direction) {
    Eigen::VectorXd values2 = values;
    values2[direction] += h;
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
}

// TEST(constraint_jac_structure, happy) {
//   std::vector<double> constraints_times{0.0, 2.0 / 3.0, 7.0 / 4.0, 2.0};
//   std::vector<double> controls_times{0, 1, 2};

//   constexpr Eigen::Index constraints_inner_length = 3;
//   constexpr Eigen::Index controls_inner_length = 5;
//   Aux::InterpolatingVector constraints(
//       constraints_times, constraints_inner_length);
//   Aux::InterpolatingVector controls(controls_times, controls_inner_length);

//   auto triplets = Optimization::constraint_jac_triplets(constraints,
//   controls); Eigen::SparseMatrix<double, Eigen::RowMajor> spmat(
//       constraints.get_total_number_of_values(),
//       controls.get_total_number_of_values());
//   spmat.setFromTriplets(triplets.begin(), triplets.end());

//   size_t triplet_index = 0;
//   for (Eigen::Index outer_row = 0; outer_row != constraints.size();
//        ++outer_row) {
//     auto it = std::lower_bound(
//         controls_times.begin(), controls_times.end(),
//         constraints_times[static_cast<size_t>(outer_row)]);
//     for (Eigen::Index inner_row = 0; inner_row != constraints_inner_length;
//          ++inner_row) {
//       Eigen::Index lastControlIndex
//           = static_cast<Eigen::Index>(it - controls_times.begin());
//       for (Eigen::Index outer_col = 0; outer_col <= lastControlIndex;
//            ++outer_col) {
//         for (Eigen::Index inner_col = 0; inner_col != controls_inner_length;
//              ++inner_col) {
//           EXPECT_EQ(
//               triplets[triplet_index].row(),
//               outer_row * constraints_inner_length + inner_row);
//           EXPECT_EQ(
//               triplets[triplet_index].col(),
//               outer_col * controls_inner_length + inner_col);
//           ++triplet_index;
//         }
//       }
//     }
//   }
//   EXPECT_EQ(triplet_index, triplets.size());
// }

// TEST(lolo, laoa) {
//   Eigen::SparseMatrix<double, Eigen::RowMajor> jac(5, 8);

//   Eigen::SparseVector<double, Eigen::RowMajor> vec(8);
//   for (int i = 0; i != 8; ++i) { vec.coeffRef(i) = 1; }

//   jac.row(2) = vec;

//   std::cout << Eigen::MatrixXd(jac) << std::endl;
// }
