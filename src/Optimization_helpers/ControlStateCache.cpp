#include "ControlStateCache.hpp"
#include "Controlcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Timeevolver.hpp"

namespace Optimization {

  ControlStateCache::ControlStateCache(
      Model::Timeevolver &timeevolver, Model::Controlcomponent &_problem) :
      evolver(timeevolver), problem(_problem) {}

  Aux::InterpolatingVector_Base const *ControlStateCache::compute_states(
      Aux::InterpolatingVector_Base const &controls,
      std::vector<double> state_interpolation_points,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state) {
    if (last_failed == controls) {
      return nullptr;
    }
    if (controls == cache.first) {
      return &cache.second;
    } else {
      Aux::InterpolatingVector states(
          state_interpolation_points, initial_state.size());
      try {
        evolver.simulate(initial_state, controls, problem, states);
      } catch (...) {
        last_failed = controls;
        return nullptr;
      }
      Aux::InterpolatingVector cached_controls(controls);

      cache = std::make_pair(cached_controls, std::move(states));
      return &cache.second;
    }
  }
} // namespace Optimization
