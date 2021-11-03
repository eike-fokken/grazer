#include "ControlToStates.hpp"
#include "Exception.hpp"
#include "Timeevolver.hpp"

namespace Optimization {

  ControlToStates::ControlToStates(
      Model::Timeevolver &timeevolver, Model::Controlcomponent &problem,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state) :
      evolver(timeevolver), problem(problem), initial_state(initial_state) {}

  bool ControlToStates::get_states(
      Aux::InterpolatingVector const &controls,
      Aux::InterpolatingVector &states) {
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
