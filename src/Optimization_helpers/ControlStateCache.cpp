#include "ControlStateCache.hpp"
#include "Controlcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Timeevolver.hpp"
#include <type_traits>
#include <utility>

namespace Optimization {

  ControlStateCache::ControlStateCache(
      Model::Timeevolver &timeevolver, Model::Controlcomponent &_problem) :
      evolver(timeevolver), problem(_problem) {}

  bool ControlStateCache::refresh_cache(
      Aux::InterpolatingVector_Base const &controls,
      Eigen::Ref<Eigen::VectorXd const> const &state_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state) {

    Aux::InterpolatingVector states(state_timepoints, initial_state.size());
    try {
      evolver.simulate(initial_state, controls, problem, states);
    } catch (...) {
      failed = Cacheentry{controls, state_timepoints, initial_state};
      return false;
    }
    cache = std::make_pair(
        Cacheentry{controls, state_timepoints, initial_state},
        std::move(states));
    return true;
  }

  Aux::InterpolatingVector_Base const &ControlStateCache::get_cached_states() {
    return cache.second;
  }

  Aux::InterpolatingVector_Base const *
  ControlStateCache::check_and_supply_states(
      Aux::InterpolatingVector_Base const &controls,
      Eigen::Ref<Eigen::VectorXd const> const &state_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state) {
    if (failed.control == controls
        and failed.state_timepoints == state_timepoints
        && failed.initial_state == initial_state) {
      return nullptr;
    }
    if (controls == cache.first.control
        and initial_state == cache.first.initial_state
        && state_timepoints == cache.first.state_timepoints) {
      return &cache.second;
    } else {
      Aux::InterpolatingVector states(state_timepoints, initial_state.size());
      try {
        evolver.simulate(initial_state, controls, problem, states);
      } catch (...) {
        failed = Cacheentry{controls, state_timepoints, initial_state};
        return nullptr;
      }

      cache = std::make_pair(
          Cacheentry{controls, state_timepoints, initial_state},
          std::move(states));
      return &cache.second;
    }
  }

} // namespace Optimization
