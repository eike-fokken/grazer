#include "EquationDerivativeCache.hpp"
#include "Controlcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Timeevolver.hpp"
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Optimization {

  EquationDerivativeCache::EquationDerivativeCache() {}

  std::tuple<SolverVector const &, MatrixVector const &, MatrixVector const &>
  EquationDerivativeCache::factorizations(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state,
      Model::Controlcomponent &problem) {

    // cache entry differs:
    if (not(controls == entry.control and initial_state == entry.initial_state
            and states.get_interpolation_points()
                    == entry.state_interpolation_points)) {
      // fill the constraints vector for every (constraints-)timepoint
      for (Eigen::Index states_timeindex = 1; states_timeindex != states.size();
           ++states_timeindex) {
        double time = states.interpolation_point_at_index(states_timeindex);
        // problem.d_evaluate(
        //         .mut_timestep(states_timeindex), time, states(time),
        //         controls(time));
      }
    }
    return std::make_tuple(
        std::ref(dE_dnew_state_solvers), std::ref(dE_dlast_state),
        std::ref(dE_dcontrol));
  }

} // namespace Optimization
