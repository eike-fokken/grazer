#include "ControlStateCache.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Timeevolver.hpp"

namespace Optimization {

  ControlStateCache::ControlStateCache(
      Model::Timeevolver &timeevolver, Model::Controlcomponent &_problem) :
      evolver(timeevolver), problem(_problem) {}

  bool ControlStateCache::get_states(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base &states,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state) {
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
      Aux::InterpolatingVector cached_controls(controls);
      Aux::InterpolatingVector cached_states(states);

      cache = std::make_pair(cached_controls, cached_states);
      return true;
    }
  }
} // namespace Optimization
