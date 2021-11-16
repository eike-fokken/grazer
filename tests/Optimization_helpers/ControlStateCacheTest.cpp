#include "ControlStateCache.hpp"
#include "Controlcomponent.hpp"
#include "InterpolatingVector.hpp"
#include "Mock_Controlcomponent.hpp"
#include "Timeevolver.hpp"
#include <gtest/gtest.h>
#include <iostream>

using namespace testing;

TEST(ControlStateCache, first) {

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

  auto evolver = Model::Timeevolver::make_instance(timeevolution_json);
  TestControlComponent_for_ControlStateCache p;

  Optimization::ControlStateCache a(evolver, p);

  std::vector<double> times(2);
  times[0] = 0;
  times[1] = 1;
  Eigen::VectorXd initial(2);
  initial << 5, 6;
  Aux::InterpolatingVector controls(times, 2);
  Eigen::VectorXd controlvalues(4);
  controlvalues << 2, 3, 4, 5;
  controls.set_values_in_bulk(controlvalues);

  auto *states_pointer = a.compute_states(controls, times, initial);

  Aux::InterpolatingVector_Base const &states = *states_pointer;
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
