/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#include "Optimization_helpers.hpp"
#include "InterpolatingVector.hpp"
#include "Mathfunctions.hpp"
#include "Misc.hpp"
#include "OptimizableObject.hpp"
#include <algorithm>
#include <cassert>
namespace Optimization {

  std::pair<Eigen::Index, double> compute_index_lambda(
      Eigen::Ref<Eigen::VectorXd const> const &timepoints, double time) {
    assert(timepoints.size() > 0);
    assert(time >= front(timepoints));
    assert(time <= back(timepoints) + Aux::EPSILON);
    assert(std::is_sorted(timepoints.begin(), timepoints.end()));

    if (time > *(std::prev(timepoints.end()))
        and time < *(std::prev(timepoints.end())) + Aux::EPSILON) {
      Eigen::Index i_upper = back_index(timepoints);
      double lambda = 1.0;
      return {i_upper, lambda};
    }

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

  Eigen::VectorX<std::pair<Eigen::Index, double>> compute_index_lambda_vector(
      Eigen::Ref<Eigen::VectorXd const> const &coarse_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &fine_timepoints) {
    Eigen::VectorX<std::pair<Eigen::Index, double>> results(
        fine_timepoints.size());

    for (Eigen::Index index = 0; index != fine_timepoints.size(); ++index) {
      double time = fine_timepoints[index];
      results[index] = compute_index_lambda(coarse_timepoints, time);
    }
    return results;
  }

  void initialize_bounds(
      Model::OptimizableObject &problem,
      Aux::InterpolatingVector_Base &lower_bounds,
      nlohmann::json const &lower_bounds_json,
      Aux::InterpolatingVector_Base &upper_bounds,
      nlohmann::json const &upper_bounds_json,
      Aux::InterpolatingVector_Base &constraints_lower_bounds,
      nlohmann::json const &constraints_lower_bounds_json,
      Aux::InterpolatingVector_Base &constraints_upper_bounds,
      nlohmann::json const &constraints_upper_bounds_json) {
    problem.set_lower_bounds(lower_bounds, lower_bounds_json);
    problem.set_upper_bounds(upper_bounds, upper_bounds_json);
    problem.set_constraint_lower_bounds(
        constraints_lower_bounds, constraints_lower_bounds_json);
    problem.set_constraint_upper_bounds(
        constraints_upper_bounds, constraints_upper_bounds_json);
  }

} // namespace Optimization
