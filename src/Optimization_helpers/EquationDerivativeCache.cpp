#include "EquationDerivativeCache.hpp"
#include "Controlcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Matrixhandler.hpp"
#include "Timeevolver.hpp"
#include <Eigen/src/SparseCore/SparseMatrix.h>
#include <Eigen/src/SparseLU/SparseLU.h>
#include <cstddef>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Optimization {

  EquationDerivativeCache::EquationDerivativeCache() {}

  void EquationDerivativeCache::initialize(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state,
      Model::Controlcomponent &problem) {

    for (Eigen::Index states_index = 1; states_index != states.size();
         ++states_index) {

      auto u_timeindex = static_cast<size_t>(states_index - 1);

      Eigen::SparseMatrix<double> dE_dlast_state_matrix(
          states.get_inner_length(), states.get_inner_length());
      Aux::Triplethandler last_handler(dE_dlast_state_matrix);

      Eigen::SparseMatrix<double> dE_dnew_matrix(
          states.get_inner_length(), states.get_inner_length());
      Aux::Triplethandler new_handler(dE_dnew_matrix);

      ////////////////////////////////////////////////////////////////////
      /// columns is the number of states because we are not yet on control time
      /// steps.
      ////////////////////////////////////////////////////////////////////
      Eigen::SparseMatrix<double> dE_dcontrol_matrix(
          states.get_inner_length(), states.get_inner_length());
      Aux::Triplethandler control_handler(dE_dcontrol_matrix);

      //////////////////////////////
      /// here fill the matrices:
      //////////////////////////////

      auto new_time = states.interpolation_point_at_index(states_index);
      auto last_time = states.interpolation_point_at_index(states_index - 1);
      auto new_state = states.vector_at_index(states_index);
      auto last_state = states.vector_at_index(states_index - 1);

      auto control = controls(new_time);

      problem.d_evalutate_d_new_state(
          new_handler, last_time, new_time, last_state, new_state, control);
      problem.d_evalutate_d_last_state(
          last_handler, last_time, new_time, last_state, new_state, control);
      problem.d_evalutate_d_control(
          control_handler, last_time, new_time, last_state, new_state, control);

      /////////////////////////////////////////////
      /// save the computed matrices and  solver states to the cache:
      /////////////////////////////////////////////

      last_handler.set_matrix();
      control_handler.set_matrix();

      new_handler.set_matrix();
      Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
      solver.compute(dE_dnew_matrix);

      dE_dnew_state_solvers.push_back(std::move(solver));
      dE_dlast_state.push_back(dE_dlast_state_matrix);
      dE_dcontrol.push_back(dE_dcontrol_matrix);
    }

    // hier die Länge der Vektoren prüfen!
    auto rightsize
        = states.size() - 1; // minus one because at the initial values
                             // (state_index == 0) no derivatives are needed.

    auto u_rightsize = static_cast<size_t>(rightsize);
    assert(dE_dnew_state_solvers.size() == u_rightsize);
    assert(dE_dlast_state.size() == u_rightsize);
    assert(dE_dcontrol.size() == u_rightsize);

    initialized = true;
  }

  std::tuple<SolverVector const &, MatrixVector const &, MatrixVector const &>
  EquationDerivativeCache::compute_derivatives(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state,
      Model::Controlcomponent &problem) {

    // cache entry differs:
    if (not(controls == entry.control and initial_state == entry.initial_state
            and states.get_interpolation_points()
                    == entry.state_interpolation_points)) {

      if (not initialized) {
        initialize(controls, states, initial_state, problem);
      }

      assert(dE_dlast_state.size() == static_cast<size_t>(states.size() - 1));

      // fill the constraints vector for every (constraints-)timepoint
      // Once again start at the second state timepoint, as the first one is the
      // (fixed) initial state.

      for (Eigen::Index states_timeindex = 1; states_timeindex != states.size();
           ++states_timeindex) {
        auto u_timeindex = static_cast<size_t>(states_timeindex - 1);
        dE_dlast_state[u_timeindex]

            double time
            = states.interpolation_point_at_index(states_timeindex);
        problem.d_evalutate_d_last_state(
            Aux::Matrixhandler & jacobianhandler, double last_time,
            double new_time,
            const Eigen::Ref<const Eigen::VectorXd> &last_state,
            const Eigen::Ref<const Eigen::VectorXd> &new_state,
            const Eigen::Ref<const Eigen::VectorXd> &control)
      }
    }
    return std::make_tuple(
        std::ref(dE_dnew_state_solvers), std::ref(dE_dlast_state),
        std::ref(dE_dcontrol));
  }

} // namespace Optimization
