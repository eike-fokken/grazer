#include "Optimization_helpers.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Networkproblem.hpp"
#include "Timeevolver.hpp"
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

} // namespace Optimization
