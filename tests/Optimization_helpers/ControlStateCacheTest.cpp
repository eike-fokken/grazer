#include "ControlStateCache.hpp"
#include "Controlcomponent.hpp"
#include "InterpolatingVector.hpp"
#include "Mock_Controlcomponent.hpp"
#include "Timeevolver.hpp"
#include "test_io_helper.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace testing;

static Eigen::VectorXd
f(Eigen::Ref<Eigen::VectorXd const> const &last_state,
  Eigen::Ref<Eigen::VectorXd const> const &new_state,
  Eigen::Ref<Eigen::VectorXd const> const &control) {
  return new_state - last_state - control;
}

static Eigen::SparseMatrix<double>
df(Eigen::Ref<Eigen::VectorXd const> const &,
   Eigen::Ref<Eigen::VectorXd const> const &,
   Eigen::Ref<Eigen::VectorXd const> const &) {
  Eigen::SparseMatrix<double> A(2, 2);
  A.setIdentity();
  return A;
}

static Eigen::SparseMatrix<double> dfdummy(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &) {
  throw std::runtime_error("This function must not be called!");
}

static Eigen::VectorXd ffail(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &) {
  throw std::runtime_error("fail!!");
}

static Eigen::SparseMatrix<double> dffail(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &) {
  Eigen::SparseMatrix<double> A(2, 2);
  A.setIdentity();
  return A;
}

TEST(ControlStateCache, fill_and_use_the_cache) {

  nlohmann::json timeevolution_json = R"(    {
        "use_simplified_newton": true,
        "maximal_number_of_newton_iterations": 2,
        "tolerance": 1e-8,
        "retries": 0,
        "start_time": 0.0,
        "end_time": 1.0,
        "desired_delta_t": 1.0
    }
)"_json;

  auto evolver_ptr
      = Model::Timeevolver::make_pointer_instance(timeevolution_json);
  TestControlComponent_for_ControlStateCache problem(f, df, dfdummy, dfdummy);

  Optimization::ControlStateCache cache(std::move(evolver_ptr));

  Eigen::VectorXd times{{0, 1}};

  Eigen::VectorXd initial(2);
  initial << 5, 6;
  Aux::InterpolatingVector controls(times, 2);
  Eigen::VectorXd controlvalues(4);
  controlvalues << 2, 3, 4, 5;
  controls.set_values_in_bulk(controlvalues);

  // Make sure that a simulation took place by expecting a call to
  // prepare_timestep! This should very much be done with googlemocks
  // EXPECT_CALL, but I don't know how.
  Aux::InterpolatingVector_Base const *states_pointer = nullptr;
  std::string output = "";
  std::string expected_output = "Called prepare_timestep";
  {
    std::stringstream buffer;
    Catch_cout catcher(buffer.rdbuf());
    states_pointer
        = cache.check_and_supply_states(problem, controls, times, initial);

    output = buffer.str();
  }
  std::cout << output;
  int encountered = 0;
  size_t current_index = 0;
  current_index = output.find(expected_output, current_index);
  while (current_index != std::string::npos) {
    ++encountered;
    current_index += expected_output.length();
    current_index = output.find(expected_output, current_index);
  }

  EXPECT_EQ(encountered, 1);
  Aux::InterpolatingVector states = *states_pointer;
  for (Eigen::Index j = 0; j != states.get_inner_length(); ++j) {
    EXPECT_DOUBLE_EQ(states(0)[j], initial[j]);
  }

  for (Eigen::Index i = 1; i != states.size(); ++i) {
    auto di = static_cast<double>(i);
    for (Eigen::Index j = 0; j != states.get_inner_length(); ++j) {
      EXPECT_DOUBLE_EQ(states(di)[j], initial[j] + controls(di)[j]);
    }
  }
  // Check that the second time the cache is used by making sure,
  // prepare_timestep is not called again. Also this should be done with
  // EXPECT_CALL but I still don't know, how.
  encountered = 0;
  output = "";
  current_index = 0;
  Aux::InterpolatingVector_Base const *new_states;
  {
    std::stringstream buffer;
    Catch_cout catcher(buffer.rdbuf());
    new_states
        = cache.check_and_supply_states(problem, controls, times, initial);
    output = buffer.str();
  }
  std::cout << output;
  current_index = output.find(expected_output, current_index);
  while (current_index != std::string::npos) {
    ++encountered;
    current_index += expected_output.length();
    current_index = output.find(expected_output, current_index);
  }
  // expect the cache to be used:
  EXPECT_EQ(encountered, 0);
  // expect the result to be equal:
  EXPECT_EQ(states, *new_states);
}

TEST(ControlStateCache, refresh_cache_then_get_states) {

  nlohmann::json timeevolution_json = R"(    {
        "use_simplified_newton": true,
        "maximal_number_of_newton_iterations": 2,
        "tolerance": 1e-8,
        "retries": 0,
        "start_time": 0.0,
        "end_time": 1.0,
        "desired_delta_t": 1.0
    }
)"_json;

  auto evolver = Model::Timeevolver::make_pointer_instance(timeevolution_json);
  TestControlComponent_for_ControlStateCache problem(f, df, dfdummy, dfdummy);

  Optimization::ControlStateCache cache(std::move(evolver));

  Eigen::VectorXd times{{0, 1}};

  Eigen::VectorXd initial(2);
  initial << 5, 6;
  Aux::InterpolatingVector controls(times, 2);
  Eigen::VectorXd controlvalues(4);
  controlvalues << 2, 3, 4, 5;
  controls.set_values_in_bulk(controlvalues);

  // Make sure that a simulation took place by expecting a call to
  // prepare_timestep! This should very much be done with googlemocks
  // EXPECT_CALL, but I don't know how.
  cache.refresh_cache(problem, controls, times, initial);
  auto &states = cache.get_cached_states();
  for (Eigen::Index j = 0; j != states.get_inner_length(); ++j) {
    EXPECT_DOUBLE_EQ(states(0)[j], initial[j]);
  }

  for (Eigen::Index i = 1; i != states.size(); ++i) {
    auto di = static_cast<double>(i);
    for (Eigen::Index j = 0; j != states.get_inner_length(); ++j) {
      EXPECT_DOUBLE_EQ(states(di)[j], initial[j] + controls(di)[j]);
    }
  }
}

TEST(ControlStateCache, failed_simulation) {
  nlohmann::json timeevolution_json = R"(    {
        "use_simplified_newton": true,
        "maximal_number_of_newton_iterations": 2,
        "tolerance": 1e-8,
        "retries": 0,
        "start_time": 0.0,
        "end_time": 1.0,
        "desired_delta_t": 1.0
    }
)"_json;

  auto evolver = Model::Timeevolver::make_pointer_instance(timeevolution_json);
  TestControlComponent_for_ControlStateCache problem(
      ffail, dffail, dfdummy, dfdummy);

  Optimization::ControlStateCache cache(std::move(evolver));

  Eigen::VectorXd times{{0, 1}};

  Eigen::VectorXd initial(2);
  initial << 5, 6;
  Aux::InterpolatingVector controls(times, 2);
  Eigen::VectorXd controlvalues(4);
  controlvalues << 2, 3, 4, 5;
  controls.set_values_in_bulk(controlvalues);

  Aux::InterpolatingVector_Base const *states_pointer = nullptr;

  // Make sure that a simulation took place by expecting a call to
  // prepare_timestep! This should very much be done with googlemocks
  // EXPECT_CALL, but I don't know how.
  std::string output = "";
  std::string expected_output = "Called prepare_timestep";
  {
    std::stringstream buffer;
    Catch_cout catcher(buffer.rdbuf());
    states_pointer
        = cache.check_and_supply_states(problem, controls, times, initial);

    output = buffer.str();
  }
  std::cout << output;
  int encountered = 0;
  size_t current_index = 0;
  current_index = output.find(expected_output, current_index);
  while (current_index != std::string::npos) {
    ++encountered;
    current_index += expected_output.length();
    current_index = output.find(expected_output, current_index);
  }

  EXPECT_EQ(encountered, 1);

  states_pointer
      = cache.check_and_supply_states(problem, controls, times, initial);
  EXPECT_EQ(states_pointer, nullptr);

  // Check that the second time the cache for failed simulations is used by
  // making sure, prepare_timestep is not called again. Also this should be done
  // with EXPECT_CALL but I still don't know, how.
  encountered = 0;
  output = "";
  current_index = 0;
  Aux::InterpolatingVector_Base const *new_states;
  {
    std::stringstream buffer;
    Catch_cout catcher(buffer.rdbuf());
    new_states
        = cache.check_and_supply_states(problem, controls, times, initial);
    output = buffer.str();
  }
  std::cout << output;
  current_index = output.find(expected_output, current_index);
  while (current_index != std::string::npos) {
    ++encountered;
    current_index += expected_output.length();
    current_index = output.find(expected_output, current_index);
  }
  // expect the cache to be used:
  EXPECT_EQ(encountered, 0);
  EXPECT_EQ(new_states, nullptr);
}
