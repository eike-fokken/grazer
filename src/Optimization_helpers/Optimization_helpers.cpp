#include "Optimization_helpers.hpp"
#include "InterpolatingVector.hpp"
#include "Networkproblem.hpp"
#include <algorithm>
#include <cassert>

namespace Optimization {

  std::pair<Eigen::Index, double> compute_index_lambda(
      Eigen::Ref<Eigen::VectorXd const> const &timepoints, double time) {
    assert(time >= *timepoints.begin());
    assert(time <= *(std::prev(timepoints.end())));
    assert(std::is_sorted(timepoints.begin(), timepoints.end()));
    // first element greater or equal to time:
    auto it_upper
        = std::lower_bound(timepoints.begin(), timepoints.end(), time);
    Eigen::Index i_upper = it_upper - timepoints.begin();
    auto time_upper = *it_upper;
    if (it_upper == timepoints.begin()) {
      // In this case we must not compute it_lower, as it would lie outside the
      // range of timepoints!
      double lambda = 1.0;
      return {i_upper, lambda};
    }
    auto it_lower = std::prev(it_upper);
    auto time_lower = *it_lower;

    // compute the convex combination coefficient lambda:
    double lambda = (time - time_lower) / (time_upper - time_lower);
    return {i_upper, lambda};
  }

  void initialize(
      Model::Networkproblem &problem, Aux::InterpolatingVector_Base &controls,
      nlohmann::json const &control_json,
      Eigen::Ref<Eigen::VectorXd> init_state,
      nlohmann::json const &initial_json,
      Aux::InterpolatingVector_Base &lower_bounds,
      nlohmann::json const &lower_bounds_json,
      Aux::InterpolatingVector_Base &upper_bounds,
      nlohmann::json const &upper_bounds_json,
      Aux::InterpolatingVector_Base &constraints_lower_bounds,
      nlohmann::json const &constraints_lower_bounds_json,
      Aux::InterpolatingVector_Base &constraints_upper_bounds,
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
