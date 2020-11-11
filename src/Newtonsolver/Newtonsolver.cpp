#include <Matrixhandler.hpp>
#include <Newtonsolver.hpp>
#include <Problem.hpp>

namespace Solver {

  void Newtonsolver::update_jabobian_triplets(
      Model::Problem &problem, double last_time, double new_time,
      Eigen::VectorXd const &last_state, Eigen::VectorXd const &new_state) {
    Aux::Triplethandler handle(&jacobian);
    Aux::Triplethandler *const handle_ptr(&handle);
    problem.evaluate_state_derivative(handle_ptr, last_time, new_time,
                                      last_state, new_state);
    handle.set_matrix();
  }
  void Newtonsolver::update_jabobian_coeffref(
      Model::Problem &problem, double last_time, double new_time,
      Eigen::VectorXd const &last_state, Eigen::VectorXd const &new_state) {

    Aux::Coeffrefhandler handle(&jacobian);
    Aux::Coeffrefhandler *const handle_ptr(&handle);
    problem.evaluate_state_derivative(handle_ptr, last_time, new_time,
                                      last_state, new_state);
  }
} // namespace Solver
