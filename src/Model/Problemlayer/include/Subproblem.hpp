#pragma once
#include <Eigen/Sparse>

namespace Model {

class Subproblem {

public:
  virtual ~Subproblem() {};


  // purely virtual functions:
  virtual void evaluate(const Eigen::VectorXd &current_state,
                        Eigen::VectorXd &new_state) = 0;
  virtual void evaluate_state_derivative(const Eigen::VectorXd &,
                                         Eigen::SparseMatrix<double> &) = 0;
  virtual unsigned int get_number_of_states() = 0;

  // Reserves indices from the state vector
  // @param int next_free_index the first non-reserved index of the state
  // vector.
  // @returns int next_free_index the new first non-reserved index of the state
  // vector.
  unsigned int reserve_indices(unsigned int next_free_index);

  unsigned int get_start_state_index();
  unsigned int get_after_state_index();

  // void set_sporadic_state_indices(std::vector<unsigned int> indices);

protected:
  unsigned int start_state_index;
  unsigned int after_state_index;
  // std::vector<unsigned int> sporadic_state_indices;

  // have to think about the implementation of connecting problems.




  // int start_modeleq_index;
  // int end_modeleq_index;
  // std::vector<int> sporadic_modeleq_indices;
};

} // namespace Model
