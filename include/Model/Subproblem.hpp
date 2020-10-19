#pragma once
#include <Eigen/Sparse>

namespace Model {

class Subproblem {

public:
  virtual ~Subproblem();


  // purely virtual functions:
  virtual void evaluate(const Eigen::VectorXd &current_state,
                        Eigen::VectorXd &new_state) = 0;
  virtual void evaluate_state_derivative(const Eigen::VectorXd &,
                                         Eigen::SparseMatrix<double> &) = 0;
  virtual int get_number_of_states();

  virtual int get_after_continuous_state_index() = 0;


  // Reserves indices from the state vector
  // @param int next_free_index the first non-reserved index of the state
  // vector.
  // @returns int next_free_index the new first non-reserved index of the state
  // vector.
  int reserve_indices(int next_free_index);



  int get_start_continuous_state_index();
  int get_end_continuous_state_index();

  void set_sporadic_state_indices(std::vector<int> indices);

protected:
  int start_continuous_state_index;
  int after_continuous_state_index;
  std::vector<int> sporadic_state_indices;

  // have to think about the implementation of connecting problems.




  // int start_continuous_modeleq_index;
  // int end_continuous_modeleq_index;
  // std::vector<int> sporadic_modeleq_indices;
};

} // namespace Model
