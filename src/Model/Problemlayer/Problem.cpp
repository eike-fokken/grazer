#include <Problem.hpp>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

namespace Aux {

  class Matrixhandler;
}

namespace Model {

  // class Model;

  void Problem::add_subproblem(std::unique_ptr<Subproblem> subproblem_ptr) {
    subproblems.push_back(std::move(subproblem_ptr));
  }

  int Problem::set_indices() {
    int next_free_index(0);
    for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
      next_free_index = (*it)->set_indices(next_free_index);
    }
    return next_free_index;
  }

  void Problem::evaluate(Eigen::VectorXd &rootfunction, double last_time,
                         double new_time, Eigen::VectorXd const &last_state,
                         Eigen::VectorXd const &new_state) {
    for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
      (*it)->evaluate(rootfunction, last_time, new_time, last_state, new_state);
    }
  }

  void Problem::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                          double last_time, double new_time,
                                          Eigen::VectorXd const &last_state,
                                          Eigen::VectorXd const &new_state) {
    for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
      (*it)->evaluate_state_derivative(jacobianhandler, last_time, new_time,
                                       last_state, new_state);
    }
  }

  std::vector<Subproblem *> Problem::get_subproblems() const {
    std::vector<Subproblem *> pointer_vector;
    for (auto &uptr : subproblems) {
      pointer_vector.push_back(uptr.get());
    }
    return pointer_vector;
  }

  void Problem::display() const {
    for (auto &subproblem_ptr : subproblems) {
      subproblem_ptr->display();
    }
  }

} // namespace Model
