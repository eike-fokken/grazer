#include "Wrapper.hpp"
#include "InterpolatingVector.hpp"
#include "Mock_OptimizableObject.hpp"

#include "Timeevolver.hpp"
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Optimization;

TEST(IpoptWrapper, getters) {

  //
  nlohmann::json timeevolver_data = R"(    {
        "use_simplified_newton": true,
        "maximal_number_of_newton_iterations": 2,
        "tolerance": 1e-8,
        "retries": 0,
        "start_time": 0.0,
        "end_time": 1.0,
        "desired_delta_t": 1.0
    }
)"_json;

  auto evolver = Model::Timeevolver::make_instance(timeevolver_data);
  Mock_OptimizableObject problem;
  problem.set_state_indices(0);
  problem.set_control_indices(0);
  problem.set_constraint_indices(0);
  Eigen::VectorXd state_timepoints{{0, 1, 2}};
  Eigen::VectorXd control_timepoints{{1, 2}};
  Eigen::VectorXd constraint_timepoints{{1, 2}};
  Eigen::VectorXd initial_state{{0, 10.0, 20.0}};
  Aux::InterpolatingVector initial_controls(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector lower_bounds(
      constraint_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector upper_bounds(
      constraint_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector constraint_lower_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());
  Aux::InterpolatingVector constraint_upper_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());

  IpoptWrapper wrapper(
      evolver, problem, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);
}
