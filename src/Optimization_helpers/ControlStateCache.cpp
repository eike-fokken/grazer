#include "ControlStateCache.hpp"
#include "Exception.hpp"
#include "Timeevolver.hpp"

namespace Optimization {

  ControlStateCache::ControlStateCache(
      Model::Timeevolver &timeevolver, Model::Controlcomponent &problem) :
      evolver(timeevolver), problem(problem) {}

  bool ControlStateCache::get_states(
      Aux::InterpolatingVector const &controls,
      Aux::InterpolatingVector &states,
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
      cache = std::make_pair(controls, states);
      return true;
    }
  }
} // namespace Optimization
