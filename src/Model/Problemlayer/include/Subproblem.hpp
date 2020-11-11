#pragma once
#include <Eigen/Sparse>

namespace Model {

  class Subproblem {

  public:
    virtual ~Subproblem(){};

    // purely virtual functions:
    virtual void evaluate(Eigen::VectorXd &rootfunction, double last_time,
                          double new_time, Eigen::VectorXd const &last_state,
                          Eigen::VectorXd const &new_state) = 0;
    virtual void
    evaluate_state_derivative(Eigen::SparseMatrix<double> &jacobian,
                              double last_time, double new_time,
                              Eigen::VectorXd const &,
                              Eigen::VectorXd const &new_state) = 0;

    // Reserves indices from the state vector
    // @param int next_free_index the first non-reserved index of the state
    // vector.
    // @returns int next_free_index the new first non-reserved index of the
    // state vector.

    unsigned int set_indices(unsigned int const next_free_index);

    unsigned int get_number_of_states() const;
    unsigned int get_start_state_index() const;
    unsigned int get_after_state_index() const;

    // void set_sporadic_state_indices(std::vector<unsigned int> indices);

  private:
    unsigned int start_state_index{0};
    unsigned int after_state_index{0};
    // std::vector<unsigned int> sporadic_state_indices;

    /// This function should reserve indices, e.g. by setting start and end
    /// indices of subobjects
    /// @param The current smallest non-reserved index.
    /// @returns new smallest non-reserved index.
    virtual unsigned int
    reserve_indices(unsigned int const next_free_index) = 0;

    // have to think about the implementation of connecting problems.

    // int start_modeleq_index;
    // int end_modeleq_index;
    // std::vector<int> sporadic_modeleq_indices;
  };

} // namespace Model
