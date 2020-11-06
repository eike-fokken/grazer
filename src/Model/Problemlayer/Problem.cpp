#include <Problem.hpp>
#include <Subproblem.hpp>
#include <memory>
#include <vector>

namespace Model {

// class Model;

void Problem::add_subproblem(std::unique_ptr<Subproblem> subproblem_ptr) {
  subproblems.push_back(std::move(subproblem_ptr));
}

unsigned int Problem::set_indices() {
  unsigned int next_free_index(0);
  for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
    next_free_index = (*it)->set_indices(next_free_index);
  }
  return next_free_index;
}

  void Problem::evaluate(Eigen::VectorXd &rootfunction, double current_time,
                       double next_time, const Eigen::VectorXd &last_state,
                       Eigen::VectorXd &current_state) {
  for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
    (*it)->evaluate(rootfunction,current_time, next_time, last_state,
                    current_state);
  }
}

void Problem::evaluate_state_derivative(Eigen::SparseMatrix<double> & jacobian, double current_time, double next_time, const Eigen::VectorXd &last_state,
                                      Eigen::VectorXd & current_state ) {
  for (auto it = subproblems.begin(); it != subproblems.end(); it++) {
    (*it)->evaluate_state_derivative(jacobian, current_time, next_time,last_state, current_state);
  }
}

  std::vector<Subproblem*> Problem::get_subproblems()
  {
    std::vector<Subproblem *> pointer_vector;
    for (auto & uptr : subproblems) {
      pointer_vector.push_back(uptr.get());
    }
    return pointer_vector;
  }

} // namespace Model
