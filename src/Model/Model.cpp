#include <Model.hpp>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

namespace Model {

// class Model;

void Model::add_subproblem(Subproblem subproblem) {
  subproblems.push_back(std::make_unique<Subproblem>(subproblem));
}

void Model::reserve_indices() {
  int next_free_index(0);
  for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
    next_free_index = (*it)->reserve_indices(next_free_index);
    // TODO: work on sporadic indices.
  }
}

void Model::evaluate(const Eigen::VectorXd &current_state,
                     Eigen::VectorXd &new_state) {
  for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
    (*it)->evaluate(current_state, new_state);
  }
}

void Model::evaluate_state_derivative(const Eigen::VectorXd &current_state,
                                      Eigen::SparseMatrix<double> &jacobian) {
  for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
    (*it)->evaluate_state_derivative(current_state, jacobian);
  }
}

} // namespace Model
