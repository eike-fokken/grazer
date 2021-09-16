#include <Eigen/Sparse>
#include <vector>

namespace optimization {

  /* \brief computes the Lagrange multipliers for the adjoint method.
   *
   * @param[out] multipliers The vector of Lagrange multipliers to be computed
   * @param[in] AT_vector vector of matrices that are the transpose of the
   * derivative of the model equations with respect to the last state.
   * @param[in] BT_vector vector of matrices that are the transpose of the
   * derivative of the model equations with respect to the new state.
   * @param[in] df_dx_vector derivative of the objective function (or inequality
   * value) with respect to the new state.
   */
  void compute_multiplier(
      std::vector<Eigen::VectorXd> &multipliers,
      std::vector<Eigen::SparseMatrix<double>> const &AT_vector,
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::VectorXd> const &df_dx_vector);
} // namespace optimization
