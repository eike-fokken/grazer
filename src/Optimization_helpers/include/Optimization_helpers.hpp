#include <Eigen/Dense>
#include <nlohmann/json.hpp>

namespace Aux {
  class InterpolatingVector_Base;
}

namespace Model {
  class Networkproblem;
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

  /** \brief Takes care of all initialization
   */
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
      nlohmann::json const &constraints_upper_bounds_json);

} // namespace Optimization
