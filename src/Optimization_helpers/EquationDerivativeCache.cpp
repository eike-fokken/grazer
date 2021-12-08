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
      Model::Controlcomponent &problem) {

    // We now exactly how many derivatives we need so we reserve:
    auto usize = static_cast<size_t>(states.size());
    dE_dcontrol.reserve(usize);
    dE_dlast_state.reserve(usize);
    dE_dnew_state_solvers.reserve(usize);

    // insert dummy matrices/solvers at index 0, so that the state_indices and
    // the derivative indices are the same.
    assert(dE_dcontrol.empty());
    assert(dE_dlast_state.empty());
    assert(dE_dnew_state_solvers.empty());

    dE_dcontrol.push_back(Eigen::SparseMatrix<double>());
    dE_dlast_state.push_back(Eigen::SparseMatrix<double>());
    dE_dnew_state_solvers.push_back(
        Eigen::SparseLU<Eigen::SparseMatrix<double>>());

    for (Eigen::Index states_index = 1; states_index != states.size();
         ++states_index) {

      auto u_timeindex = static_cast<size_t>(states_index);

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
      dE_dlast_state.push_back(std::move(dE_dlast_state_matrix));
      dE_dcontrol.push_back(std::move(dE_dcontrol_matrix));
    }

    // Check the length of the vectors:
    assert(dE_dnew_state_solvers.size() == usize);
    assert(dE_dlast_state.size() == usize);
    assert(dE_dcontrol.size() == usize);

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
        initialize(controls, states, problem);
      }

      assert(dE_dlast_state.size() == static_cast<size_t>(states.size()));

      // fill the constraints vector for every (constraints-)timepoint
      // Once again start at the second state timepoint, as the first one is the
      // (fixed) initial state.

      for (Eigen::Index states_timeindex = 1; states_timeindex != states.size();
           ++states_timeindex) {
        auto u_timeindex = static_cast<size_t>(states_timeindex);
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
