#include "Optimization_helpers.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Matrixhandler.hpp"
#include "Networkproblem.hpp"
#include "Timeevolver.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>

namespace Optimization {

  void initialize_solvers(
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers) {
    assert(BT_vector.size() == solvers.size());
    for (std::size_t index = 1; index != BT_vector.size(); ++index) {
      solvers[index].analyzePattern(BT_vector[index]);
    }
  }

  /**
   * @brief Solves the adjoint equations (5.14). Note that N + 1 ==
   * AT_vector.size() == BT_vector.size()
   *
   * @param    multipliers          the adjoint multipliers
   * @param    AT_vector            vector of transposed matrices A_1^t, ..,
   * A_N^t
   * @param    BT_vector            vector of transposed matrices B_1^t, ...,
   * B_N^t
   * @param    df_dx_vector         vector of partial derivatives f_y0, ...,
   * f_yN
   * @param    solvers              vector of Eigen solver objects
   */
  void compute_multiplier(
      bool already_factorized, std::vector<Eigen::VectorXd> &multipliers,
      std::vector<Eigen::SparseMatrix<double>> const &AT_vector,
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::VectorXd> const &df_dx_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers) {

    // The algorithm needs at least the starting and ending time step:
    assert(multipliers.size() >= 2);

    // The following is just a sanity check.
    assert(multipliers.size() == AT_vector.size());
    assert(multipliers.size() == BT_vector.size());
    assert(multipliers.size() == df_dx_vector.size());
    assert(multipliers.size() == solvers.size());

    auto N = multipliers.size() - 1;
    if (not already_factorized) {
      solvers[N].factorize(BT_vector.back());
    }
    multipliers[N] = solvers[N].solve(-1 * df_dx_vector[N]);

    for (auto k = N - 1; k >= 1; --k) {
      if (not already_factorized) {
        solvers[k].factorize(BT_vector[k]);
      }
      multipliers[k] = solvers[k].solve(
          -1 * (df_dx_vector[k] + AT_vector[k + 1] * multipliers[k + 1]));
    }
    // last iteration, j = 0
    multipliers[0] = -1.0 * (df_dx_vector[0] + AT_vector[1] * multipliers[1]);
  }

  void compute_state_from_controls(
      Model::Timeevolver &timeevolver, Model::Networkproblem &problem,
      std::vector<Eigen::VectorXd> states,
      Eigen::Ref<Eigen::VectorXd const> const &controls) {}

  void initialize(
      Model::Networkproblem &problem, Aux::InterpolatingVector &controls,
      nlohmann::json const &control_json,
      Eigen::Ref<Eigen::VectorXd> init_state,
      nlohmann::json const &initial_json,
      Aux::InterpolatingVector &lower_bounds,
      nlohmann::json const &lower_bounds_json,
      Aux::InterpolatingVector &upper_bounds,
      nlohmann::json const &upper_bounds_json,
      Aux::InterpolatingVector &constraints_lower_bounds,
      nlohmann::json const &constraints_lower_bounds_json,
      Aux::InterpolatingVector &constraints_upper_bounds,
      nlohmann::json const &constraints_upper_bounds_json) {
    problem.set_initial_values(init_state, initial_json);
    problem.set_initial_controls(controls, control_json);
    problem.set_lower_bounds(lower_bounds, lower_bounds_json);
    problem.set_upper_bounds(upper_bounds, upper_bounds_json);
    problem.set_constraint_lower_bounds(
        constraints_lower_bounds, constraints_lower_bounds_json);
    problem.set_constraint_upper_bounds(
        constraints_upper_bounds, constraints_upper_bounds_json);
  }

  void d_constraints_d_constraint_time_control(
      std::vector<Aux::Matrixhandler> &constraint_step_handler,
      Model::Constraintcomponent &problem,
      std::vector<double> const &constraint_times,
      Aux::InterpolatingVector const &controls,
      Aux::InterpolatingVector const &states) {
    assert(constraint_step_handler.size() == constraint_times.size());
    for (size_t index = 0; index != constraint_times.size(); ++index) {
      problem.d_evaluate_constraint_d_control(
          constraint_step_handler[index], constraint_times[index],
          states(constraint_times[index]), controls(constraint_times[index]));
    }
  }

  static void add_homotethy_triplets_at_index(
      std::vector<Eigen::Triplet<double, Eigen::Index>> &triplets,
      double scale_factor, Eigen::Index row, Eigen::Index column,
      Eigen::Index size) {
    for (Eigen::Index i = 0; i != size; ++i) {
      Eigen::Triplet<double, Eigen::Index> newtriplet(
          row + i, column + i, scale_factor);
      triplets.push_back(newtriplet);
    }
  }

  Eigen::SparseMatrix<double> compute_control_conversion(
      Aux::InterpolatingVector const &fine_resolution_vector,
      Aux::InterpolatingVector const &coarse_resolution_vector) {

    auto const &fine_times = fine_resolution_vector.get_interpolation_points();
    auto const &coarse_times
        = coarse_resolution_vector.get_interpolation_points();
    if (fine_resolution_vector.get_inner_length()
        != coarse_resolution_vector.get_inner_length()) {
      gthrow(
          {"The function ", __func__,
           " can only be called on InterpolatingVectors with equal sizes of "
           "vectors at each point."});
    }
    if (fine_times.front() < coarse_times.front()) {
      gthrow(
          {"Some interpolation points of the finer resolved "
           "InterpolatingVector are smaller than the smallest interpolation "
           "point of the coarse vector."});
    }
    if (fine_times.back() > coarse_times.back()) {
      gthrow(
          {"Some interpolation points of the finer resolved "
           "InterpolatingVector are greater than the greatest interpolation "
           "point of the coarse vector."});
    }

    Eigen::SparseMatrix<double> jacobian(
        fine_resolution_vector.get_total_number_of_values(),
        coarse_resolution_vector.get_total_number_of_values());

    std::vector<Eigen::Triplet<double, Eigen::Index>> triplets;

    Eigen::Index inner_matrix_size = fine_resolution_vector.get_inner_length();
    for (size_t i = 0; i != fine_times.size(); ++i) {

      auto t_j_interator = std::lower_bound(
          coarse_times.begin(), coarse_times.end(), fine_times[i]);
      auto j = t_j_interator - coarse_times.begin();

      if (j == 0) {
        // This can only happen on the first of the fine time points, therefore
        // row and column index are 0 and lambda = 0.
        add_homotethy_triplets_at_index(triplets, 1.0, 0, 0, inner_matrix_size);
      } else {
        auto t = fine_times[i];
        auto t_jm1 = *std::prev(t_j_interator);
        auto t_j = *t_j_interator;
        auto lambda = (t - t_jm1) / (t_j - t_jm1);
        if (lambda != 1) {
          add_homotethy_triplets_at_index(
              triplets, 1 - lambda,
              static_cast<Eigen::Index>(i) * inner_matrix_size,
              static_cast<Eigen::Index>(j - 1) * inner_matrix_size,
              inner_matrix_size);
        }
        if (lambda != 0) {
          add_homotethy_triplets_at_index(
              triplets, lambda,
              static_cast<Eigen::Index>(i) * inner_matrix_size,
              static_cast<Eigen::Index>(j) * inner_matrix_size,
              inner_matrix_size);
        }
      }
    }
    jacobian.setFromTriplets(triplets.begin(), triplets.end());
    return jacobian;
  }

} // namespace Optimization
