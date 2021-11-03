#include "ControlToStates.hpp"
#include "Exception.hpp"
#include "Timeevolver.hpp"

namespace Optimization {

  ControlToStates::ControlToStates(Model::Timeevolver &timeevolver) :
      evolver(timeevolver) {}

  bool ControlToStates::get_states(
      Aux::InterpolatingVector const &controls,
      Aux::InterpolatingVector &states, Eigen::VectorXd const &initial_state,
      Model::Networkproblem &problem) {
    if (last_failed == controls) {
      return false;
    }
    if (controls == cache.first) {
      states = cache.second;
      return true;
    } else {

      try {
        evolver.simulate(initial_state, controls, problem, states);
      } catch (...) {
        last_failed = controls;
        return false;
      }
      cache = std::make_pair(controls, states);
      return true;
    }
  }
} // namespace Optimization
