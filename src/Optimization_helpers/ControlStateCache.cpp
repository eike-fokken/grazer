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

  Aux::InterpolatingVector_Base const *ControlStateCache::compute_states(
      Aux::InterpolatingVector_Base const &controls,
      std::vector<double> const &state_interpolation_points,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state) {
    if (failed.control == controls
        and failed.state_interpolation_points == state_interpolation_points
        && failed.initial_state == initial_state) {
      return nullptr;
    }
    if (controls == cache.first.control
        and initial_state == cache.first.initial_state
        && state_interpolation_points
               == cache.first.state_interpolation_points) {
      std::cout << "cache used" << std::endl;
      return &cache.second;
    } else {
      Aux::InterpolatingVector states(
          state_interpolation_points, initial_state.size());
      try {
        evolver.simulate(initial_state, controls, problem, states);
      } catch (...) {
        failed
            = Cacheentry{controls, state_interpolation_points, initial_state};
        return nullptr;
      }
      Aux::InterpolatingVector cached_controls = controls;

      cache = std::make_pair(
          Cacheentry{
              cached_controls, state_interpolation_points, initial_state},
          std::move(states));
      std::cout << "cache filled!" << std::endl;
      return &cache.second;
    }
  }

} // namespace Optimization
