#define EIGEN_RUNTIME_NO_MALLOC // Define this symbol to enable runtime tests
                                // for allocations
#include "Wrapper.hpp"
#include "InterpolatingVector.hpp"
#include "Mock_OptimizableObject.hpp"
#include "Timeevolver.hpp"

#include <gmock/gmock-matchers.h>
#include <gtest/gtest-death-test.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Optimization;

TEST(IpoptWrapper, simple_dimension_getters) {

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

  Eigen::Index const number_of_states(3);
  Eigen::Index const number_of_controls(2);
  Eigen::Index const number_of_constraints(1);

  Mock_OptimizableObject problem(
      number_of_states, number_of_controls, number_of_constraints);

  problem.set_state_indices(0);
  problem.set_control_indices(0);
  problem.set_constraint_indices(0);

  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{1, 2, 3}};
  Eigen::VectorXd constraint_timepoints{{2, 3}};

  Eigen::VectorXd initial_state{{0, 10.0, 20.0}};
  Aux::InterpolatingVector initial_controls(
      control_timepoints, problem.get_number_of_controls_per_timepoint());

  Aux::InterpolatingVector lower_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector upper_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector constraint_lower_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());
  Aux::InterpolatingVector constraint_upper_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());

  IpoptWrapper wrapper(
      evolver, problem, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);

  EXPECT_EQ(wrapper.state_steps(), 4);
  EXPECT_EQ(wrapper.control_steps(), 3);
  EXPECT_EQ(wrapper.constraint_steps(), 2);

  EXPECT_EQ(wrapper.states_per_step(), 3);
  EXPECT_EQ(wrapper.controls_per_step(), 2);
  EXPECT_EQ(wrapper.constraints_per_step(), 1);

  EXPECT_EQ(
      wrapper.get_total_no_constraints(),
      wrapper.constraints_per_step() * wrapper.constraint_steps());
  EXPECT_EQ(
      wrapper.get_total_no_controls(),
      wrapper.controls_per_step() * wrapper.control_steps());
}

TEST(IpoptWrapper, Matrix_blocks) {

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
  Eigen::Index const number_of_states(30);
  Eigen::Index const number_of_controls(20);
  Eigen::Index const number_of_constraints(10);

  Mock_OptimizableObject problem(
      number_of_states, number_of_controls, number_of_constraints);
  problem.set_state_indices(0);
  problem.set_control_indices(0);
  problem.set_constraint_indices(0);

  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{1, 2, 3}};
  Eigen::VectorXd constraint_timepoints{{2, 3}};

  Eigen::VectorXd initial_state(number_of_states);
  double value = 1;
  for (auto &entry : initial_state) {
    entry = 10 * value;
    ++value;
  }
  Aux::InterpolatingVector initial_controls(
      control_timepoints, problem.get_number_of_controls_per_timepoint());

  Aux::InterpolatingVector lower_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector upper_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector constraint_lower_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());
  Aux::InterpolatingVector constraint_upper_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());

  IpoptWrapper wrapper(
      evolver, problem, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);

  RowMat dg_dui(
      wrapper.get_total_no_constraints(), wrapper.controls_per_step());

  for (auto i = dg_dui.data();
       i != dg_dui.data() + dg_dui.rows() * dg_dui.rows(); ++i) {
    *i = static_cast<double>(i - dg_dui.data()) + 1;
  }
  {
    Eigen::internal::set_is_malloc_allowed(false);
    auto a = wrapper.middle_row_block(dg_dui, 0);
    Eigen::internal::set_is_malloc_allowed(true);

    Eigen::internal::set_is_malloc_allowed(false);
    auto b = wrapper.middle_row_block(dg_dui, 1);
    Eigen::internal::set_is_malloc_allowed(true);

    std::cout << a << "\n\n" << b << std::endl;
  }

  {

    std::cout << "lower_rows:\n";
    Eigen::internal::set_is_malloc_allowed(false);
    auto a = wrapper.lower_rows(dg_dui, 0);
    Eigen::internal::set_is_malloc_allowed(true);

    Eigen::internal::set_is_malloc_allowed(false);
    auto b = wrapper.lower_rows(dg_dui, 1);
    Eigen::internal::set_is_malloc_allowed(true);

    std::cout << a << "\n\n" << b << std::endl;
  }
}

TEST(IpoptWrapperDeathTest, matrix) {
  GTEST_FLAG_SET(death_test_style, "threadsafe");
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
  Eigen::Index const number_of_states(30);
  Eigen::Index const number_of_controls(20);
  Eigen::Index const number_of_constraints(10);

  Mock_OptimizableObject problem(
      number_of_states, number_of_controls, number_of_constraints);
  problem.set_state_indices(0);
  problem.set_control_indices(0);
  problem.set_constraint_indices(0);

  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{1, 2, 3}};
  Eigen::VectorXd constraint_timepoints{{2, 3}};

  Eigen::VectorXd initial_state(number_of_states);
  double value = 1;
  for (auto &entry : initial_state) {
    entry = 10 * value;
    ++value;
  }
  Aux::InterpolatingVector initial_controls(
      control_timepoints, problem.get_number_of_controls_per_timepoint());

  Aux::InterpolatingVector lower_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector upper_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector constraint_lower_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());
  Aux::InterpolatingVector constraint_upper_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());

  IpoptWrapper wrapper(
      evolver, problem, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);

  RowMat dg_dui(
      wrapper.get_total_no_constraints(), wrapper.controls_per_step());

  EXPECT_DEATH(wrapper.lower_rows(dg_dui, -1), "outer_row_index.*>=.*0");
  EXPECT_DEATH(
      wrapper.lower_rows(dg_dui, 2), "outer_row_index.*<.*constraint_steps()");
}
