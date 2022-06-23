/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#include "Newtonsolver.hpp"
#include "Controlcomponent.hpp"
#include "Exception.hpp"
#include "Matrixhandler.hpp"
#include <sstream>
#include <string>

namespace Solver {

  Newtonsolver::Newtonsolver(double _tolerance, int _maximal_iterations) :
      tolerance(_tolerance), maximal_iterations(_maximal_iterations) {}

  void Newtonsolver::evaluate_state_derivative_triplets(
      Model::Controlcomponent const &problem, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) {

    {
      jacobian.resize(new_state.size(), new_state.size());
      Aux::Triplethandler handler(jacobian);

      problem.d_evalutate_d_new_state(
          handler, last_time, new_time, last_state, new_state, control);
      handler.set_matrix();
    }
    lusolver.analyzePattern(jacobian);
  }

  void Newtonsolver::evaluate_state_derivative_coeffref(
      Model::Controlcomponent const &problem, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) {
    Aux::Coeffrefhandler handler(jacobian);
    problem.d_evalutate_d_new_state(
        handler, last_time, new_time, last_state, new_state, control);
  }

  Eigen::Index Newtonsolver::get_number_non_zeros_jacobian() {
    return jacobian.nonZeros();
  }

  Eigen::Index Newtonsolver::get_dimension_of_jacobian() {
    return jacobian.cols();
  }
  Solutionstruct Newtonsolver::solve(
      Eigen::Ref<Eigen::VectorXd> new_state,
      Model::Controlcomponent const &problem, bool newjac,
      bool use_full_jacobian, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) {
    Solutionstruct solstruct;

    Eigen::VectorXd rootvalues(new_state.size());

    // compute f(x_k);
    problem.evaluate(
        rootvalues, last_time, new_time, last_state, new_state, control);

    // check if already there:
    if (rootvalues.norm() <= tolerance) {
      solstruct.success = true;
      solstruct.residual = rootvalues.norm();
      solstruct.used_iterations = 0;
      return solstruct;
    }

    // compute f'(x_k) and write it to the jacobian.
    if (newjac) {
      evaluate_state_derivative_triplets(
          problem, last_time, new_time, last_state, new_state, control);
    } else {
      evaluate_state_derivative_coeffref(
          problem, last_time, new_time, last_state, new_state, control);
    }
    if (not use_full_jacobian) {
      lusolver.factorize(jacobian);
      if (lusolver.info() != Eigen::Success) {
        std::ostringstream o;
        o << "Couldn't decompose a Jacobian, it may be non-invertible.\n "
          << "time: " << std::to_string(new_time)
          << "\n Note, that only LU decomposition is implemented.\n";
        throw SolverNumericalProblem(o.str());
      }
    }
    while (rootvalues.norm() > tolerance
           && solstruct.used_iterations < maximal_iterations) {
      if (use_full_jacobian) {
        evaluate_state_derivative_coeffref(
            problem, last_time, new_time, last_state, new_state, control);
        lusolver.factorize(jacobian);
        if (lusolver.info() != Eigen::Success) {
          std::ostringstream o;
          o << "Couldn't decompose a Jacobian, it may be non-invertible.\n "
            << "time: " << std::to_string(new_time)
            << "\n Note, that only LU decomposition is implemented.\n";
          throw SolverNumericalProblem(o.str());
        }
      }
      // compute Dx_k:
      Eigen::VectorXd step = -lusolver.solve(rootvalues);

      double lambda = 1.0;
      // candidate for x_{k+1}
      Eigen::VectorXd candidate_vector = new_state + lambda * step;

      // f(x_{k+1}
      Eigen::VectorXd candidate_values(new_state.size());
      problem.evaluate(
          candidate_values, last_time, new_time, last_state, candidate_vector,
          control);

      // Delta^bar x_k+1
      Eigen::VectorXd delta_x_bar = -lusolver.solve(candidate_values);

      double current_norm = delta_x_bar.norm();

      double testnorm = step.norm();
      while (current_norm > (1 - 0.5 * lambda) * testnorm) {
        if (current_norm < minimal_stepsize) {
          std::ostringstream o;
          o << " Minimal stepsize reached at time: "
            << std::to_string(new_time);
          throw SolverNumericalProblem(o.str());
        }
        lambda *= 0.5;
        candidate_vector = new_state + lambda * step;
        problem.evaluate(
            candidate_values, last_time, new_time, last_state, candidate_vector,
            control);
        current_norm = (-lusolver.solve(candidate_values)).norm();
      }
      new_state = candidate_vector;
      rootvalues = candidate_values;
      ++solstruct.used_iterations;
      solstruct.residual = rootvalues.norm();
    }
    if (solstruct.used_iterations == maximal_iterations
        and solstruct.residual > tolerance) {
      solstruct.success = false;
    } else {
      solstruct.success = true;
    }

    return solstruct;
  }

} // namespace Solver
