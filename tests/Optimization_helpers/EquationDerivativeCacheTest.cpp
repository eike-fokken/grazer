#include "EquationDerivativeCache.hpp"
#include "Controlcomponent.hpp"
#include "InterpolatingVector.hpp"
#include "Mock_Controlcomponent.hpp"
#include "test_io_helper.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace testing;

static Eigen::VectorXd
f(Eigen::VectorXd last_state, Eigen::VectorXd new_state,
  Eigen::VectorXd control) {
  return new_state - last_state - 2 * control;
}

static Eigen::SparseMatrix<double>
df_dnew(Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd) {
  std::cout << "Called df_dnew!" << std::endl;
  Eigen::SparseMatrix<double> A(2, 2);
  A.setIdentity();
  return A;
}

static Eigen::SparseMatrix<double>
df_dlast(Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd) {
  std::cout << "Called df_dlast!" << std::endl;
  Eigen::SparseMatrix<double> A(2, 2);
  A.setIdentity();
  return -A;
}

static Eigen::SparseMatrix<double>
df_dcontrol(Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd) {
  std::cout << "Called df_dcontrol!" << std::endl;
  Eigen::SparseMatrix<double> A(2, 2);
  A.setIdentity();
  return -2 * A;
}

TEST(EquationDerivativeCache, happy) {
  Optimization::EquationDerivativeCache cache(2);

  TestControlComponent_for_ControlStateCache problem(
      f, df_dnew, df_dlast, df_dcontrol);
  std::vector<double> times(2);
  times[0] = 0;
  times[1] = 1;
  Eigen::VectorXd initial(2);
  initial << 5, 6;
  Aux::InterpolatingVector controls(times, 2);
  Eigen::VectorXd controlvalues(4);
  controlvalues << 2, 3, 4, 5;
  controls.set_values_in_bulk(controlvalues);

  Aux::InterpolatingVector states(times, 2);
  Eigen::VectorXd statevalues(4);
  statevalues << 2, 3, 4, 5;
  states.set_values_in_bulk(statevalues);

  auto [dE_dnew_state_solver, dE_dlast_state, dE_dcontrol]
      = cache.compute_derivatives(controls, states, problem);

  auto [dE_dnew_state_solver_cached, dE_dlast_state_cached, dE_dcontrol_cached]
      = cache.compute_derivatives(controls, states, problem);

  for (size_t index = 1; index != times.size(); ++index) {

    Eigen::Matrix2d Id;
    Id.setIdentity();

    // Make sure the correct solution is obtained:
    Eigen::MatrixXd last = dE_dlast_state[index];
    EXPECT_EQ(last, -Id);
    Eigen::MatrixXd control = dE_dcontrol[index];
    EXPECT_EQ(control, -2 * Id);

    Eigen::Vector2d b{1.0, 2.0};
    Eigen::Vector2d c{2.0, 1.0};
    EXPECT_EQ(b, dE_dnew_state_solver[index].solve(b));
    EXPECT_EQ(c, dE_dnew_state_solver[index].solve(c));

    // Make sure the caching makes the same output:
    Eigen::MatrixXd last_cached = dE_dlast_state_cached[index];
    EXPECT_EQ(last, last_cached);
    Eigen::MatrixXd control_cached = dE_dcontrol_cached[index];
    EXPECT_EQ(control, control_cached);
    EXPECT_EQ(b, dE_dnew_state_solver_cached[index].solve(b));
    EXPECT_EQ(c, dE_dnew_state_solver_cached[index].solve(c));
  }
}

TEST(EquationDerivativeCache, caching) {
  // Make sure the derivatives were computed. This should very much be done with
  // googlemocks EXPECT_CALL, but I don't know how. Therefore we listen for the
  // correct output.
  Optimization::EquationDerivativeCache cache(2);

  TestControlComponent_for_ControlStateCache problem(
      f, df_dnew, df_dlast, df_dcontrol);
  std::vector<double> times(2);
  times[0] = 0;
  times[1] = 1;
  Eigen::VectorXd initial(2);
  initial << 5, 6;
  Aux::InterpolatingVector controls(times, 2);
  Eigen::VectorXd controlvalues(4);
  controlvalues << 2, 3, 4, 5;
  controls.set_values_in_bulk(controlvalues);

  Aux::InterpolatingVector states(times, 2);
  Eigen::VectorXd statevalues(4);
  statevalues << 2, 3, 4, 5;
  states.set_values_in_bulk(statevalues);

  {
    std::string output = "";
    std::string expected_output = "Called df_dnew!";

    {
      std::stringstream buffer;
      Catch_cout catcher(buffer.rdbuf());
      cache.compute_derivatives(controls, states, problem);

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
  }
  {
    std::string output = "";
    std::string expected_output = "Called df_dnew!";

    {
      std::stringstream buffer;
      Catch_cout catcher(buffer.rdbuf());
      cache.compute_derivatives(controls, states, problem);

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

    EXPECT_EQ(encountered, 0);
  }
}
