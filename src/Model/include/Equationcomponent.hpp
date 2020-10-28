#pragma once
#include <Eigen/Sparse>

namespace Model {



class Equationcomponent
{

public:

  virtual ~Equationcomponent();

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

protected:
  unsigned int start_state_index;
  unsigned int after_state_index;

};

} // namespace Model
