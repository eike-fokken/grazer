#include "EquationDerivativeCache.hpp"
#include "Controlcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Matrixhandler.hpp"
#include "Timeevolver.hpp"
#include <cstddef>
#include <iostream>
#include <tuple>
#include <type_traits>
#include <utility>

namespace Optimization {

  // Because the solver class is non-copyable AND non-movable we need to default
  // contstruct the correct number of solvers beforehand:
  EquationDerivativeCache::EquationDerivativeCache(
      Eigen::Index number_of_states) :
      dE_dnew_state_solvers(static_cast<size_t>(number_of_states)),
      dE_dlast_state(static_cast<size_t>(number_of_states)),
      dE_dcontrol(static_cast<size_t>(number_of_states)) {}

  void EquationDerivativeCache::initialize_derivatives(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states,
      Model::Controlcomponent &problem) {

    auto usize = static_cast<size_t>(states.size());
    // Check the length of the vectors:
    assert(dE_dnew_state_solvers.size() == usize);
    assert(dE_dlast_state.size() == usize);
    assert(dE_dcontrol.size() == usize);

    dE_dnew_matrix.resize(states.get_inner_length(), states.get_inner_length());

    for (Eigen::Index states_index = 1; states_index != states.size();
         ++states_index) {
      auto current_unsigned_index = static_cast<size_t>(states_index);

      Eigen::SparseMatrix<double> &dE_dlast_state_matrix
          = dE_dlast_state[current_unsigned_index];
      dE_dlast_state_matrix.resize(
          states.get_inner_length(), states.get_inner_length());
      Aux::Triplethandler last_handler(dE_dlast_state_matrix);

      ////////////////////////////////////////////////////////////////////
      /// columns is the number of states because we are not yet on control time
      /// steps.
      ////////////////////////////////////////////////////////////////////
      Eigen::SparseMatrix<double> &dE_dcontrol_matrix
          = dE_dcontrol[current_unsigned_index];
      dE_dcontrol_matrix.resize(
          states.get_inner_length(), states.get_inner_length());
      Aux::Triplethandler control_handler(dE_dcontrol_matrix);

      Aux::Triplethandler new_handler(dE_dnew_matrix);

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
      Eigen::SparseLU<Eigen::SparseMatrix<double>> &solver
          = dE_dnew_state_solvers[current_unsigned_index];
      solver.compute(dE_dnew_matrix);
    }

    initialized = true;
  }

  void EquationDerivativeCache::update_derivatives(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states,
      Model::Controlcomponent &problem) {

    auto usize = static_cast<size_t>(states.size());
    // Check the length of the vectors:
    assert(dE_dnew_state_solvers.size() == usize);
    assert(dE_dlast_state.size() == usize);
    assert(dE_dcontrol.size() == usize);

    for (Eigen::Index states_index = 1; states_index != states.size();
         ++states_index) {
      auto current_unsigned_index = static_cast<size_t>(states_index);

      Eigen::SparseMatrix<double> &dE_dlast_state_matrix
          = dE_dlast_state[current_unsigned_index];
      assert(dE_dlast_state_matrix.rows() == states.get_inner_length());
      assert(dE_dlast_state_matrix.cols() == states.get_inner_length());
      Aux::Coeffrefhandler last_handler(dE_dlast_state_matrix);

      ////////////////////////////////////////////////////////////////////
      /// columns is the number of states because we are not yet on control time
      /// steps.
      ////////////////////////////////////////////////////////////////////
      Eigen::SparseMatrix<double> &dE_dcontrol_matrix
          = dE_dcontrol[current_unsigned_index];

      assert(dE_dcontrol_matrix.rows() == states.get_inner_length());
      assert(dE_dcontrol_matrix.cols() == states.get_inner_length());
      Aux::Coeffrefhandler control_handler(dE_dcontrol_matrix);

      //////////////////////////////////////////////////////
      // Careful: The following relies on the fact that each entry, that can
      // ever be non-zero is set in each run!
      //
      // Also it is vital that the non-zero structure doesn't change from
      // timestep to timestep!
      //
      // Both of these are the case up to now (December 21).
      //////////////////////////////////////////////////////

      assert(dE_dnew_matrix.rows() == states.get_inner_length());
      assert(dE_dnew_matrix.cols() == states.get_inner_length());
      Aux::Coeffrefhandler new_handler(dE_dnew_matrix);

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
      /// The computed matrices are in the cache, need to save the solver
      /// states:
      /////////////////////////////////////////////

      Eigen::SparseLU<Eigen::SparseMatrix<double>> &solver
          = dE_dnew_state_solvers[current_unsigned_index];
      solver.compute(dE_dnew_matrix);
    }
  }

  std::tuple<SolverVector const &, MatrixVector const &, MatrixVector const &>
  EquationDerivativeCache::compute_derivatives(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state,
      Model::Controlcomponent &problem) {

    // Work only if cache entry differs:
    if (not(controls == entry.control and initial_state == entry.initial_state
            and states.get_interpolation_points()
                    == entry.state_interpolation_points)) {
      if (not initialized) {
        initialize_derivatives(controls, states, problem);
      } else { // already initialized
        update_derivatives(controls, states, problem);
      }
    }
    return std::make_tuple(
        std::ref(dE_dnew_state_solvers), std::ref(dE_dlast_state),
        std::ref(dE_dcontrol));
  }

} // namespace Optimization
