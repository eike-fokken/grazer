#include <Eigen/Sparse>
#include <vector>

namespace optimization {

  /** \brief initializes the solvers with the sparsity pattern of the respective
   * matrices.
   *
   * @param[in] BT_vector A vector of matrices whose sparsity pattern shall be
   * loaded into the solvers in #solvers.
   * @param[out] solvers A vector of LU solvers.
   */
  void initialize_solvers(
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers);

  /** \brief computes the Lagrange multipliers for the adjoint method.
   *
   * @param[out] multipliers The vector of Lagrange multipliers to be
   * computed
   * @param[in] AT_vector vector of matrices that are the transpose of the
   * derivative of the model equations with respect to the last state.
   * @param[in] BT_vector vector of matrices that are the transpose of the
   * derivative of the model equations with respect to the new state.
   * @param[in] df_dx_vector derivative of the objective function (or
   * inequality value) with respect to the new state.
   * @param[in] solvers A vector of solvers, that are initialized with the
   * right sparsity pattern for the corresponding matrix in #BT_vector
   */
  void compute_multiplier(
      std::vector<Eigen::VectorXd> &multipliers,
      std::vector<Eigen::SparseMatrix<double>> const &AT_vector,
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::VectorXd> const &df_dx_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers);
} // namespace optimization
