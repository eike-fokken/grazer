#include <Problem.hpp>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

namespace Model {

// class Model;

void Problem::add_subproblem(std::unique_ptr<Subproblem> subproblem_ptr) {
  subproblems.push_back(std::move(subproblem_ptr));
}

void Problem::reserve_indices() {
  unsigned int next_free_index(0);
  for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
    next_free_index = (*it)->reserve_indices(next_free_index);
  }
}

void Problem::evaluate(const Eigen::VectorXd &current_state,
                     Eigen::VectorXd &new_state) {
  for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
    (*it)->evaluate(current_state, new_state);
  }
}

void Problem::evaluate_state_derivative(const Eigen::VectorXd &current_state,
                                      Eigen::SparseMatrix<double> &jacobian) {
  for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
    (*it)->evaluate_state_derivative(current_state, jacobian);
  }
}

} // namespace Model
