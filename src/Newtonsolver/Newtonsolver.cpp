#include "Newtonsolver.hpp"
#include "Exception.hpp"
#include "Matrixhandler.hpp"

namespace Solver {
  template <typename Problemtype>
  Newtonsolver_temp<Problemtype>::Newtonsolver_temp(
      double _tolerance, int _maximal_iterations) :
      tolerance(_tolerance), maximal_iterations(_maximal_iterations) {}

  template <typename Problemtype>
  void Newtonsolver_temp<Problemtype>::evaluate_state_derivative_triplets(
      Problemtype &problem, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd> new_state) {

    {
      jacobian.resize(new_state.size(), new_state.size());
      Aux::Triplethandler handler(&jacobian);

      Aux::Triplethandler *const handler_ptr = &handler;
      problem.evaluate_state_derivative(
          handler_ptr, last_time, new_time, last_state, new_state);
      handler.set_matrix();
    }
    lusolver.analyzePattern(jacobian);
  }

  template <typename Problemtype>
  void Newtonsolver_temp<Problemtype>::evaluate_state_derivative_coeffref(
      Problemtype &problem, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> new_state) {
    Aux::Coeffrefhandler handler(&jacobian);
    Aux::Coeffrefhandler *const handler_ptr = &handler;
    problem.evaluate_state_derivative(
        handler_ptr, last_time, new_time, last_state, new_state);
  }

  template <typename Problemtype>
  Eigen::Index Newtonsolver_temp<Problemtype>::get_number_non_zeros_jacobian() {
    return jacobian.nonZeros();
  }

  template <typename Problemtype>
  Solutionstruct Newtonsolver_temp<Problemtype>::solve(
      Eigen::Ref<Eigen::VectorXd> new_state, Problemtype &problem, bool newjac,
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state) {
    Solutionstruct solstruct;

    Eigen::VectorXd rootvalues(new_state.size());

    // compute f(x_k);
    problem.evaluate(rootvalues, last_time, new_time, last_state, new_state);

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
          problem, last_time, new_time, last_state, new_state);
    } else {
      evaluate_state_derivative_coeffref(
          problem, last_time, new_time, last_state, new_state);
    }
    while (rootvalues.norm() > tolerance
           && solstruct.used_iterations < maximal_iterations) {
      lusolver.factorize(jacobian);
      if (lusolver.info() != Eigen::Success) {
        gthrow(
            {"Couldn't decompose a Jacobian, it may be non-invertible.\n "
             "time: ",
             std::to_string(new_time),
             "\n Note, that only LU decomposition is implemented."})
      }
      // compute Dx_k:
      Eigen::VectorXd step = -lusolver.solve(rootvalues);

      double lambda = 1.0;
      // candidate for x_{k+1}
      Eigen::VectorXd candidate_vector = new_state + lambda * step;

      // f(x_{k+1}
      Eigen::VectorXd candidate_values(new_state.size());
      problem.evaluate(
          candidate_values, last_time, new_time, last_state, candidate_vector);

      // Delta^bar x_k+1
      Eigen::VectorXd delta_x_bar = -lusolver.solve(candidate_values);

      double current_norm = delta_x_bar.norm();

      double testnorm = step.norm();
      while (current_norm > (1 - 0.5 * lambda) * testnorm) {
        if (current_norm < minimal_stepsize) {
          gthrow({" Minimal stepsize reached!"});
        }
        lambda *= 0.5;
        candidate_vector = new_state + lambda * step;
        problem.evaluate(
            candidate_values, last_time, new_time, last_state,
            candidate_vector);
        current_norm = (-lusolver.solve(candidate_values)).norm();
      }
      new_state = candidate_vector;
      rootvalues = candidate_values;
      ++solstruct.used_iterations;
      solstruct.residual = rootvalues.norm();
    }
    if (solstruct.used_iterations == maximal_iterations) {
      solstruct.success = false;
    } else {
      solstruct.success = true;
    }

    return solstruct;
  }

} // namespace Solver
