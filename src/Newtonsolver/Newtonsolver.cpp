#include <Eigen/Sparse>
#include <Matrixhandler.hpp>
#include <Newtonsolver.hpp>
#include <iostream>

namespace Solver {

  void Newtonsolver::evaluate_state_derivative_triplets(
      Model::Problem &problem, double last_time, double new_time,
      Eigen::VectorXd const &last_state, Eigen::VectorXd const &new_state) {
    {
      jacobian.resize(new_state.size(), new_state.size());
      Aux::Triplethandler handler(&jacobian);

      Aux::Triplethandler *const handler_ptr = &handler;
      problem.evaluate_state_derivative(handler_ptr, last_time, new_time,
                                        last_state, new_state);
      handler.set_matrix();
    }
    sparselusolver.analyzePattern(jacobian);
  }

  void Newtonsolver::evaluate_state_derivative_coeffref(
      Model::Problem &problem, double last_time, double new_time,
      Eigen::VectorXd const &last_state, Eigen::VectorXd const &new_state) {
    Aux::Coeffrefhandler handler(&jacobian);
    Aux::Coeffrefhandler *const handler_ptr = &handler;
    problem.evaluate_state_derivative(handler_ptr, last_time, new_time,
                                      last_state, new_state);
  }

  Solutionstruct Newtonsolver::solve(Eigen::VectorXd &new_state,
                                     Model::Problem &problem,
                                     bool new_jacobian_structure,
                                     double last_time, double new_time,
                                     Eigen::VectorXd const &last_state) {
    Solutionstruct solstruct;

    // If the structure of the jacobian changed one should generate it from
    // triplets anew:
    if (new_jacobian_structure) {
      evaluate_state_derivative_triplets(problem, last_time, new_time,
                                         last_state, new_state);
    }
    // If the non-zero elements are the same, just update them:
    else {
      evaluate_state_derivative_coeffref(problem, last_time, new_time,
                                         last_state, new_state);
    }

    Eigen::VectorXd temp1(new_state.size());
    Eigen::VectorXd temp2(new_state.size());
    Eigen::VectorXd *vec1 = &temp1;
    Eigen::VectorXd *vec2 = &temp2;
    Eigen::VectorXd *temp;

    Eigen::VectorXd function(new_state.size());
    Eigen::VectorXd step(new_state.size());
    double stepsizeparameter;
    double oldstepsizeparameter;
    double stepsize;

    *vec1 = new_state;
    problem.evaluate(function, last_time, new_time, last_state, *vec1);
    solstruct.residual = function.lpNorm<Eigen::Infinity>();

    while (solstruct.residual > tolerance &&
           solstruct.used_iterations < maximal_iterations) {
      // compute new derivative before the next step:
      problem.evaluate(function, last_time, new_time, last_state, *vec2);
      evaluate_state_derivative_coeffref(problem, last_time, new_time,
                                         last_state, *vec1);
      sparselusolver.factorize(jacobian);
      step = sparselusolver.solve(-function);
      oldstepsizeparameter = step.lpNorm<2>();
      stepsize = 2.0; // This is set to 2.0, so that in the first run of the
                      // following loop the stepsize is decreased to the correct
                      // initial value of 1.0.

      // Stepsize computation:
      do {
        stepsize *= 0.5;
        *vec2 = *vec1 + stepsize * step;
        problem.evaluate(function, last_time, new_time, last_state, *vec2);
        stepsizeparameter = sparselusolver.solve(-function).lpNorm<2>();
      } while ((stepsizeparameter >
                (1 - decrease_value * stepsize) * oldstepsizeparameter) &&
               stepsize > minimal_stepsize);

      solstruct.residual = function.lpNorm<Eigen::Infinity>();
      temp = vec1;
      vec1 = vec2; // we switch pointers in order to not copy data and still
                   // have access to the last value.
      vec2 = temp;
      ++solstruct.used_iterations;
    }
    new_state = *vec1;

    if (solstruct.used_iterations == maximal_iterations) {
      solstruct.success = false;
    } else {
      solstruct.success = true;
    }

    return solstruct;
  }

} // namespace Solver
