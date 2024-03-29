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
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

namespace Aux {
  class InterpolatingVector_Base;
}

namespace Model {
  class OptimizableObject;
} // namespace Model

namespace Optimization {

  /** \brief For a given time and a sorted vector of timepoints, computes the
   * index of the first timepoint greater or equal to time and the convex
   * combination lambda.
   *
   * lambda is such that
   * (1-lambda)timepoints[index-1] = lambda timpoints[index] = time.
   */

  std::pair<Eigen::Index, double> compute_index_lambda(
      Eigen::Ref<Eigen::VectorXd const> const &timepoints, double time);

  /** brief computes compute_index_lambda for a whole input vector of fine time
   * points.
   */
  Eigen::VectorX<std::pair<Eigen::Index, double>> compute_index_lambda_vector(
      Eigen::Ref<Eigen::VectorXd const> const &coarse_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &fine_timepoints);

  /** \brief Takes care of all initialization
   */
  void initialize_bounds(
      Model::OptimizableObject &problem,
      Aux::InterpolatingVector_Base &lower_bounds,
      nlohmann::json const &lower_bounds_json,
      Aux::InterpolatingVector_Base &upper_bounds,
      nlohmann::json const &upper_bounds_json,
      Aux::InterpolatingVector_Base &constraints_lower_bounds,
      nlohmann::json const &constraints_lower_bounds_json,
      Aux::InterpolatingVector_Base &constraints_upper_bounds,
      nlohmann::json const &constraints_upper_bounds_json);

} // namespace Optimization
