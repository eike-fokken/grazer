#define EIGEN_RUNTIME_NO_MALLOC // Define this symbol to enable runtime tests
                                // for allocations
#include "ImplicitOptimizer.hpp"
#include "ConstraintJacobian.hpp"
#include "ControlStateCache.hpp"
#include "InterpolatingVector.hpp"
#include "Mock_OptimizableObject.hpp"
#include "Mock_StateCache.hpp"
#include "Timeevolver.hpp"
#include <memory>

#include <gmock/gmock-matchers.h>
#include <gtest/gtest-death-test.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Optimization;

nlohmann::json timeevolver_data = R"(    {
        "use_simplified_newton": true,
        "maximal_number_of_newton_iterations": 2,
        "tolerance": 1e-8,
        "retries": 0,
        "start_time": 0.0,
        "end_time": 2.0,
        "desired_delta_t": 1.0
    }
)"_json;

std::unique_ptr<ImplicitOptimizer> optimizer_ptr(
    Eigen::Index number_of_states = 30, Eigen::Index number_of_controls = 20,
    Eigen::Index number_of_constraints = 10,
    Eigen::VectorXd state_timepoints = Eigen::VectorXd{{0, 1, 2, 3}},
    Eigen::VectorXd control_timepoints = Eigen::VectorXd{{0, 2, 3}},
    Eigen::VectorXd constraint_timepoints = Eigen::VectorXd{{2, 3}},
    std::unique_ptr<StateCache> cache = std::unique_ptr<StateCache>());

TEST(ImplicitOptimizer, simple_dimension_getters) {

  Eigen::Index number_of_states = 3;
  Eigen::Index number_of_controls = 2;
  Eigen::Index number_of_constraints = 1;
  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints);
  auto &optimizer = *optimizer_pointer;
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
  Eigen::Index const number_of_states(30);
  Eigen::Index const number_of_controls(20);
  Eigen::Index const number_of_constraints(10);

  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{0, 2, 3}};
  Eigen::VectorXd constraint_timepoints{{2, 3}};

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints);
  auto &optimizer = *optimizer_pointer;

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

  Eigen::Index const number_of_states(30);
  Eigen::Index const number_of_controls(20);
  Eigen::Index const number_of_constraints(10);

  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{0, 2, 3}};
  Eigen::VectorXd constraint_timepoints{{2, 3}};

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints);
  auto &optimizer = *optimizer_pointer;

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

  Eigen::Index const number_of_states(31);
  Eigen::Index const number_of_controls(23);
  Eigen::Index const number_of_constraints(11);

  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{0, 2, 3}};
  Eigen::VectorXd constraint_timepoints{{2, 3}};
  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints);
  auto &optimizer = *optimizer_pointer;

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
  Eigen::Index const number_of_states(30);
  Eigen::Index const number_of_controls(20);
  Eigen::Index const number_of_constraints(10);
  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{0, 2, 3}};
  Eigen::VectorXd constraint_timepoints{{2, 3}};

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints);
  auto &optimizer = *optimizer_pointer;

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

  Eigen::Index const number_of_states(2);
  Eigen::Index const number_of_controls(2);
  Eigen::Index const number_of_constraints(0);
  Eigen::VectorXd state_timepoints{{0, 1, 2}};
  Eigen::VectorXd control_timepoints{{0, 3}};
  Eigen::VectorXd constraint_timepoints{{}};

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints);
  auto &optimizer = *optimizer_pointer;

  Eigen::VectorXd raw_initial_controls = optimizer.get_initial_controls();
  Aux::InterpolatingVector initial_controls(
      control_timepoints, number_of_controls);
  initial_controls.set_values_in_bulk(raw_initial_controls);

  double objective = 0;

  optimizer.evaluate_objective(raw_initial_controls, objective);

  Aux::InterpolatingVector states = optimizer.get_current_full_state();
  Aux::InterpolatingVector controls(control_timepoints, number_of_controls);
  controls.set_values_in_bulk(optimizer.get_initial_controls());

  double expected_objective = default_cost(controls(1), states(1))
                              + default_cost(controls(2), states(2));
  EXPECT_DOUBLE_EQ(objective, expected_objective);
}

TEST(ImplicitOptimizer, evaluate_constraints) {
  Eigen::Index const number_of_states(2);
  Eigen::Index const number_of_controls(2);
  Eigen::Index const number_of_constraints(2);
  Eigen::VectorXd state_timepoints{{0, 1, 2}};
  Eigen::VectorXd control_timepoints{{0, 3}};
  Eigen::VectorXd constraint_timepoints{{1, 2}};

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints);
  auto &optimizer = *optimizer_pointer;

  Eigen::VectorXd raw_initial_controls = optimizer.get_initial_controls();
  Aux::InterpolatingVector initial_controls(
      control_timepoints, number_of_controls);
  initial_controls.set_values_in_bulk(raw_initial_controls);

  Eigen::VectorXd constraints(optimizer.get_total_no_constraints());
  optimizer.evaluate_constraints(raw_initial_controls, constraints);

  Aux::InterpolatingVector states = optimizer.get_current_full_state();
  Aux::InterpolatingVector controls(control_timepoints, number_of_controls);
  controls.set_values_in_bulk(optimizer.get_initial_controls());

  Aux::InterpolatingVector expected_constraints(
      constraint_timepoints, number_of_constraints);

  for (Eigen::Index i = 0; i != expected_constraints.size(); ++i) {
    double time = constraint_timepoints[i];
    expected_constraints.mut_timestep(i) = default_constraint(
        number_of_constraints, states(time), controls(time));
  }
  EXPECT_EQ(constraints, expected_constraints.get_allvalues());
}

TEST(ImplicitOptimizer, objective_gradient1) {

  auto evolver_ptr
      = Model::Timeevolver::make_pointer_instance(timeevolver_data);

  Eigen::Index const number_of_states(3);
  Eigen::Index const number_of_controls(3);
  Eigen::Index const number_of_constraints(3);
  Eigen::VectorXd state_timepoints{{0, 1, 2}};
  Eigen::VectorXd control_timepoints{{0, 3}};
  Eigen::VectorXd constraint_timepoints{{1, 2}};
  auto cache = std::make_unique<ControlStateCache>(std::move(evolver_ptr));

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints,
      std::move(cache));
  auto &optimizer = *optimizer_pointer;

  Aux::InterpolatingVector controls(control_timepoints, number_of_controls);
  controls.set_values_in_bulk(optimizer.get_initial_controls());

  double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
  double finite_difference_threshold = sqrt(epsilon);

  Eigen::VectorXd ipoptcontrols = controls.get_allvalues();
  double objective = 0;
  optimizer.evaluate_objective(ipoptcontrols, objective);
  Eigen::RowVectorXd gradient(ipoptcontrols.size());
  optimizer.evaluate_objective_gradient(ipoptcontrols, gradient);
  Eigen::VectorXd h(ipoptcontrols.size());
  for (Eigen::Index i = 0; i != h.size(); ++i) {
    h.setZero();
    h[i] = epsilon;

    optimizer.new_x();
    Eigen::VectorXd ipoptcontrols2 = ipoptcontrols + h;
    double objective2 = 0;
    optimizer.evaluate_objective(ipoptcontrols2, objective2);
    optimizer.new_x();
    auto diff_dobjective = (objective2 - objective) / epsilon;
    auto ex_dobjective = (gradient * h)(0, 0) / epsilon;
    EXPECT_NEAR(diff_dobjective, ex_dobjective, finite_difference_threshold);
  }
}

TEST(ImplicitOptimizer, constraint_jacobian1) {

  auto evolver_ptr
      = Model::Timeevolver::make_pointer_instance(timeevolver_data);

  Eigen::Index const number_of_states(3);
  Eigen::Index const number_of_controls(3);
  Eigen::Index const number_of_constraints(3);
  Eigen::VectorXd state_timepoints{{0, 1, 2}};
  Eigen::VectorXd control_timepoints{{0, 3}};
  Eigen::VectorXd constraint_timepoints{{1, 2}};
  auto cache = std::make_unique<ControlStateCache>(std::move(evolver_ptr));

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints,
      std::move(cache));
  auto &optimizer = *optimizer_pointer;

  Aux::InterpolatingVector controls(control_timepoints, number_of_controls);
  controls.set_values_in_bulk(optimizer.get_initial_controls());

  double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
  double finite_difference_threshold = sqrt(epsilon);

  Eigen::VectorXd ipoptcontrols = controls.get_allvalues();
  Eigen::VectorXd constraints(optimizer.get_total_no_constraints());
  optimizer.evaluate_constraints(ipoptcontrols, constraints);
  Eigen::VectorXd jacobian_storage(optimizer.get_no_nnz_in_jacobian());
  MappedConstraintJacobian jacobian(
      jacobian_storage.data(), jacobian_storage.size(),
      optimizer.constraints_per_step(), optimizer.controls_per_step(),
      constraint_timepoints, control_timepoints);
  optimizer.evaluate_constraint_jacobian(ipoptcontrols, jacobian_storage);
  Eigen::VectorXd h(ipoptcontrols.size());
  for (Eigen::Index i = 0; i != h.size(); ++i) {
    h.setZero();
    h[i] = epsilon;

    optimizer.new_x();
    Eigen::VectorXd ipoptcontrols2 = ipoptcontrols + h;
    Eigen::VectorXd constraints2(optimizer.get_total_no_constraints());
    optimizer.evaluate_constraints(ipoptcontrols2, constraints2);
    Eigen::VectorXd diff_dconstraints = (constraints2 - constraints) / epsilon;
    Eigen::VectorXd ex_dconstraints = jacobian.whole_matrix() * h / epsilon;
    // std::cout << "i: " << i << std::endl;
    // std::cout << "diff:\n" << diff_dconstraints << std::endl;
    // std::cout << "ex:\n" << ex_dconstraints << std::endl;
    EXPECT_NEAR(
        (diff_dconstraints - ex_dconstraints).norm(), 0,
        finite_difference_threshold);
  }
}

TEST(ImplicitOptimizer, objective_gradient2) {

  auto evolver_ptr
      = Model::Timeevolver::make_pointer_instance(timeevolver_data);

  Eigen::Index const number_of_states(3);
  Eigen::Index const number_of_controls(3);
  Eigen::Index const number_of_constraints(3);
  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{0, 1, 1.5, 3}};
  Eigen::VectorXd constraint_timepoints{{1, 2}};
  auto cache = std::make_unique<ControlStateCache>(std::move(evolver_ptr));

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints,
      std::move(cache));
  auto &optimizer = *optimizer_pointer;

  Aux::InterpolatingVector controls(control_timepoints, number_of_controls);
  controls.set_values_in_bulk(optimizer.get_initial_controls());

  double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
  // Note that in this case apparently a higher threshhold must be set.
  double finite_difference_threshold = 2 * sqrt(epsilon);

  Eigen::VectorXd ipoptcontrols = controls.get_allvalues();
  double objective = 0;
  optimizer.evaluate_objective(ipoptcontrols, objective);
  Eigen::RowVectorXd gradient(ipoptcontrols.size());
  optimizer.evaluate_objective_gradient(ipoptcontrols, gradient);
  Eigen::VectorXd h(ipoptcontrols.size());
  for (Eigen::Index i = 0; i != h.size(); ++i) {
    h.setZero();
    h[i] = epsilon;

    optimizer.new_x();
    Eigen::VectorXd ipoptcontrols2 = ipoptcontrols + h;
    double objective2 = 0;
    optimizer.evaluate_objective(ipoptcontrols2, objective2);
    optimizer.new_x();
    auto diff_dobjective = (objective2 - objective) / epsilon;
    auto ex_dobjective = (gradient * h)(0, 0) / epsilon;
    EXPECT_NEAR(diff_dobjective, ex_dobjective, finite_difference_threshold);
  }
  std::cout << optimizer.get_integral_weights();
}

TEST(ImplicitOptimizer, constraint_jacobian2) {
  auto evolver_ptr
      = Model::Timeevolver::make_pointer_instance(timeevolver_data);

  Eigen::Index const number_of_states(3);
  Eigen::Index const number_of_controls(3);
  Eigen::Index const number_of_constraints(3);
  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{0, 1.5, 3}};
  Eigen::VectorXd constraint_timepoints{{1, 2, 3}};
  auto cache = std::make_unique<ControlStateCache>(std::move(evolver_ptr));

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints,
      std::move(cache));
  auto &optimizer = *optimizer_pointer;

  Aux::InterpolatingVector controls(control_timepoints, number_of_controls);
  controls.set_values_in_bulk(optimizer.get_initial_controls());

  double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
  double finite_difference_threshold = sqrt(epsilon);

  Eigen::VectorXd ipoptcontrols = controls.get_allvalues();
  Eigen::VectorXd constraints(optimizer.get_total_no_constraints());
  optimizer.evaluate_constraints(ipoptcontrols, constraints);
  Eigen::VectorXd jacobian_storage(optimizer.get_no_nnz_in_jacobian());
  MappedConstraintJacobian jacobian(
      jacobian_storage.data(), jacobian_storage.size(),
      optimizer.constraints_per_step(), optimizer.controls_per_step(),
      constraint_timepoints, control_timepoints);
  optimizer.evaluate_constraint_jacobian(ipoptcontrols, jacobian_storage);
  Eigen::VectorXd h(ipoptcontrols.size());
  for (Eigen::Index i = 0; i != h.size(); ++i) {
    h.setZero();
    h[i] = epsilon;

    optimizer.new_x();
    Eigen::VectorXd ipoptcontrols2 = ipoptcontrols + h;
    Eigen::VectorXd constraints2(optimizer.get_total_no_constraints());
    optimizer.evaluate_constraints(ipoptcontrols2, constraints2);
    Eigen::VectorXd diff_dconstraints = (constraints2 - constraints) / epsilon;
    Eigen::VectorXd ex_dconstraints = jacobian.whole_matrix() * h / epsilon;
    // std::cout << "i: " << i << std::endl;
    // std::cout << "diff:\n" << diff_dconstraints << std::endl;
    EXPECT_NEAR(
        (diff_dconstraints - ex_dconstraints).norm(), 0,
        finite_difference_threshold);
  }
}

TEST(ImplicitOptimizer, constraint_jacobian3) {

  auto evolver_ptr
      = Model::Timeevolver::make_pointer_instance(timeevolver_data);

  Eigen::Index const number_of_states(3);
  Eigen::Index const number_of_controls(3);
  Eigen::Index const number_of_constraints(3);
  Eigen::VectorXd state_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd control_timepoints{{0, 1, 2, 3}};
  Eigen::VectorXd constraint_timepoints{{1, 2, 3}};
  auto cache = std::make_unique<ControlStateCache>(std::move(evolver_ptr));

  auto optimizer_pointer = optimizer_ptr(
      number_of_states, number_of_controls, number_of_constraints,
      state_timepoints, control_timepoints, constraint_timepoints,
      std::move(cache));
  auto &optimizer = *optimizer_pointer;

  Aux::InterpolatingVector controls(control_timepoints, number_of_controls);
  controls.set_values_in_bulk(optimizer.get_initial_controls());

  double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
  double finite_difference_threshold = sqrt(epsilon);

  Eigen::VectorXd ipoptcontrols = controls.get_allvalues();
  Eigen::VectorXd constraints(optimizer.get_total_no_constraints());
  optimizer.evaluate_constraints(ipoptcontrols, constraints);
  Eigen::VectorXd jacobian_storage(optimizer.get_no_nnz_in_jacobian());
  MappedConstraintJacobian jacobian(
      jacobian_storage.data(), jacobian_storage.size(),
      optimizer.constraints_per_step(), optimizer.controls_per_step(),
      constraint_timepoints, control_timepoints);
  optimizer.evaluate_constraint_jacobian(ipoptcontrols, jacobian_storage);
  Eigen::VectorXd h(ipoptcontrols.size());
  for (Eigen::Index i = 0; i != h.size(); ++i) {
    h.setZero();
    h[i] = epsilon;

    optimizer.new_x();
    Eigen::VectorXd ipoptcontrols2 = ipoptcontrols + h;
    Eigen::VectorXd constraints2(optimizer.get_total_no_constraints());
    optimizer.evaluate_constraints(ipoptcontrols2, constraints2);
    Eigen::VectorXd diff_dconstraints = (constraints2 - constraints) / epsilon;
    Eigen::VectorXd ex_dconstraints = jacobian.whole_matrix() * h / epsilon;
    // std::cout << "i: " << i << std::endl;
    // std::cout << "diff:\n" << diff_dconstraints << std::endl;
    EXPECT_NEAR(
        (diff_dconstraints - ex_dconstraints).norm(), 0,
        finite_difference_threshold);
    // std::cout << optimizer.get_constraint_jacobian().whole_matrix()
    //           << std::endl;
  }
}

// instance factory:
std::unique_ptr<ImplicitOptimizer> optimizer_ptr(
    Eigen::Index number_of_states, Eigen::Index number_of_controls,
    Eigen::Index number_of_constraints, Eigen::VectorXd state_timepoints,
    Eigen::VectorXd control_timepoints, Eigen::VectorXd constraint_timepoints,
    std::unique_ptr<StateCache> cache) {

  auto problem = std::make_unique<Mock_OptimizableObject>(
      number_of_states, number_of_controls, number_of_constraints);

  problem->set_state_indices(0);
  problem->set_control_indices(0);
  problem->set_constraint_indices(0);

  Eigen::VectorXd initial_state(number_of_states);
  {
    double value = 1;
    for (auto &entry : initial_state) {
      entry = value;
      ++value;
    }
  }

  Aux::InterpolatingVector initial_controls(
      control_timepoints, problem->get_number_of_controls_per_timepoint());

  Eigen::VectorXd bulk_initial_controls(
      initial_controls.get_total_number_of_values());
  {
    double value = 100;
    for (auto &entry : bulk_initial_controls) {
      entry = value;
      ++value;
    }
  }

  initial_controls.set_values_in_bulk(bulk_initial_controls);

  Aux::InterpolatingVector lower_bounds(
      control_timepoints, problem->get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector upper_bounds(
      control_timepoints, problem->get_number_of_controls_per_timepoint());
  Aux::InterpolatingVector constraint_lower_bounds(
      constraint_timepoints,
      problem->get_number_of_constraints_per_timepoint());
  Aux::InterpolatingVector constraint_upper_bounds(
      constraint_timepoints,
      problem->get_number_of_constraints_per_timepoint());

  if (cache == nullptr) {
    Aux::InterpolatingVector states(state_timepoints, number_of_states);
    Eigen::VectorXd bulk_states(states.get_total_number_of_values());
    {
      double value = 1;
      for (auto &entry : bulk_states) {
        entry = value;
        ++value;
      }
    }
    states.set_values_in_bulk(bulk_states);
    assert(states.vector_at_index(0) == initial_state);
    cache = std::make_unique<Mock_StateCache>(states);
  }

  auto optimizer_ptr = std::make_unique<ImplicitOptimizer>(
      std::move(problem), std::move(cache), state_timepoints,
      control_timepoints, constraint_timepoints, initial_state,
      initial_controls, lower_bounds, upper_bounds, constraint_lower_bounds,
      constraint_upper_bounds);
  return optimizer_ptr;
}
