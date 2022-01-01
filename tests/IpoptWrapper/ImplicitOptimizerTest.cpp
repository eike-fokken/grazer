
#define EIGEN_RUNTIME_NO_MALLOC // Define this symbol to enable runtime tests
                                // for allocations
#include "ImplicitOptimizer.hpp"
#include "ControlStateCache.hpp"
#include "InterpolatingVector.hpp"
#include "Mock_OptimizableObject.hpp"
#include "Timeevolver.hpp"

#include <gmock/gmock-matchers.h>
#include <gtest/gtest-death-test.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Optimization;

static double cost(
    Eigen::Ref<Eigen::VectorXd const> const &controls,
    Eigen::Ref<Eigen::VectorXd const> const &states) {
  return controls.norm();
}

TEST(ImplicitOptimizer, simple_dimension_getters) {

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

  ImplicitOptimizer optimizer(
      problem, evolver, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);

  EXPECT_EQ(optimizer.state_steps(), 4);
  EXPECT_EQ(optimizer.control_steps(), 3);
  EXPECT_EQ(optimizer.constraint_steps(), 2);

  EXPECT_EQ(optimizer.states_per_step(), 3);
  EXPECT_EQ(optimizer.controls_per_step(), 2);
  EXPECT_EQ(optimizer.constraints_per_step(), 1);

  EXPECT_EQ(
      optimizer.get_total_no_constraints(),
      optimizer.constraints_per_step() * optimizer.constraint_steps());
  EXPECT_EQ(
      optimizer.get_total_no_controls(),
      optimizer.controls_per_step() * optimizer.control_steps());
}

TEST(ImplicitOptimizer, Matrix_row_blocks) {

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

  ImplicitOptimizer optimizer(
      problem, evolver, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);

  RowMat dg_dui(
      optimizer.get_total_no_constraints(), optimizer.controls_per_step());

  for (auto i = dg_dui.data();
       i != dg_dui.data() + dg_dui.rows() * dg_dui.cols(); ++i) {
    *i = static_cast<double>(i - dg_dui.data()) + 1;
  }
  {
    Eigen::internal::set_is_malloc_allowed(false);
    auto a = optimizer.middle_row_block(dg_dui, 0);
    EXPECT_EQ(a, dg_dui.topRows(optimizer.constraints_per_step()));
    Eigen::internal::set_is_malloc_allowed(true);

    Eigen::internal::set_is_malloc_allowed(false);
    auto b = optimizer.middle_row_block(dg_dui, 1);
    EXPECT_EQ(
        b, dg_dui.middleRows(
               optimizer.constraints_per_step(),
               optimizer.constraints_per_step()));
    Eigen::internal::set_is_malloc_allowed(true);
  }

  {

    Eigen::internal::set_is_malloc_allowed(false);
    auto a = optimizer.lower_rows(dg_dui, 0);
    Eigen::internal::set_is_malloc_allowed(true);
    EXPECT_EQ(a, dg_dui);

    Eigen::internal::set_is_malloc_allowed(false);
    auto b = optimizer.lower_rows(dg_dui, 1);
    EXPECT_EQ(b, dg_dui.bottomRows(optimizer.constraints_per_step()));
    Eigen::internal::set_is_malloc_allowed(true);
  }
}

#ifndef NDEBUG
TEST(ImplicitOptimizerDeathTest, matrix_row_blocks) {
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
  initial_controls.setZero();
  Aux::InterpolatingVector lower_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  lower_bounds.setZero();
  Aux::InterpolatingVector upper_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  upper_bounds.setZero();
  Aux::InterpolatingVector constraint_lower_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());
  constraint_lower_bounds.setZero();
  Aux::InterpolatingVector constraint_upper_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());
  constraint_upper_bounds.setZero();

  ImplicitOptimizer optimizer(
      problem, evolver, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);

  RowMat dg_dui(
      optimizer.get_total_no_constraints(), optimizer.controls_per_step());

  // Check that asserts fail, if the outer_row_index exceeds its range:
  EXPECT_DEATH(optimizer.lower_rows(dg_dui, -1), "outer_row_index.*>=.*0");
  EXPECT_DEATH(
      optimizer.lower_rows(dg_dui, 2),
      "outer_row_index.*<.*constraint_steps()");
}
#endif

TEST(ImplicitOptimizer, Matrix_col_blocks) {

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

  ImplicitOptimizer optimizer(
      problem, evolver, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);

  Eigen::MatrixXd Lambda(
      optimizer.states_per_step(), optimizer.get_total_no_constraints());

  for (auto index = Lambda.data();
       index != Lambda.data() + Lambda.rows() * Lambda.cols(); ++index) {
    *index = static_cast<double>(index - Lambda.data()) + 1;
  }

  {
    Eigen::internal::set_is_malloc_allowed(false);
    auto a = optimizer.middle_col_block(Lambda, 0);
    EXPECT_EQ(a, Lambda.leftCols(optimizer.constraints_per_step()));
    Eigen::internal::set_is_malloc_allowed(true);
    Eigen::internal::set_is_malloc_allowed(false);
    auto b = optimizer.middle_col_block(Lambda, 1);
    EXPECT_EQ(
        b, Lambda.middleCols(
               optimizer.constraints_per_step(),
               optimizer.constraints_per_step()));
    Eigen::internal::set_is_malloc_allowed(true);
  }
  {

    Eigen::internal::set_is_malloc_allowed(false);
    auto a = optimizer.right_cols(Lambda, 0);
    Eigen::internal::set_is_malloc_allowed(true);
    EXPECT_EQ(a, Lambda);
    Eigen::internal::set_is_malloc_allowed(false);
    auto b = optimizer.right_cols(Lambda, 1);
    EXPECT_EQ(b, Lambda.rightCols(optimizer.constraints_per_step()));
    Eigen::internal::set_is_malloc_allowed(true);
  }
}

#ifndef NDEBUG
TEST(ImplicitOptimizerDeathTest, matrix_col_blocks) {
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

  ImplicitOptimizer optimizer(
      problem, evolver, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);

  Eigen::MatrixXd Lambda(
      optimizer.states_per_step(), optimizer.get_total_no_constraints());

  // Check that asserts fail, if the outer_row_index exceeds its range:
  EXPECT_DEATH(optimizer.right_cols(Lambda, -1), "outer_col_index.*>=.*0");
  EXPECT_DEATH(
      optimizer.right_cols(Lambda, 2),
      "outer_col_index.*<.*constraint_steps()");
}
#endif

TEST(ImplicitOptimizer, evaluate_cost) {
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
  Eigen::Index const number_of_states(2);
  Eigen::Index const number_of_controls(2);
  Eigen::Index const number_of_constraints(0);

  Mock_OptimizableObject problem(
      number_of_states, number_of_controls, number_of_constraints);
  problem.set_state_indices(0);
  problem.set_control_indices(0);
  problem.set_constraint_indices(0);

  Eigen::VectorXd state_timepoints{{0, 1}};
  Eigen::VectorXd control_timepoints{{1}};
  Eigen::VectorXd constraint_timepoints{{}};

  Eigen::VectorXd initial_state(number_of_states);
  double value = 1;
  for (auto &entry : initial_state) {
    entry = 10 * value;
    ++value;
  }
  Aux::InterpolatingVector initial_controls(
      control_timepoints, problem.get_number_of_controls_per_timepoint());

  initial_controls.setZero();

  Aux::InterpolatingVector lower_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector upper_bounds(
      control_timepoints, problem.get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector constraint_lower_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());
  Aux::InterpolatingVector constraint_upper_bounds(
      constraint_timepoints, problem.get_number_of_constraints_per_timepoint());

  ImplicitOptimizer optimizer(
      problem, evolver, state_timepoints, control_timepoints,
      constraint_timepoints, initial_state, initial_controls, lower_bounds,
      upper_bounds, constraint_lower_bounds, constraint_upper_bounds);

  ControlStateCache cache(evolver, problem);
  cache.refresh_cache(initial_controls, state_timepoints, initial_state);
  auto &states = cache.get_cached_states();

  optimizer.initialize_derivative_matrices(initial_controls, states);
}
