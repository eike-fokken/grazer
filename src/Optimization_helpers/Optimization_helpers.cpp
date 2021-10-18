#include "Optimization_helpers.hpp"
#include "Exception.hpp"
#include <cassert>

namespace optimization {

  void initialize_solvers(
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers) {
    assert(BT_vector.size() == solvers.size());
    for (std::size_t index = 1; index != BT_vector.size(); ++index) {
      solvers[index].analyzePattern(BT_vector[index]);
    }
  }

  /**
   * @brief Solves the adjoint equations (5.14). Note that N + 1 ==
   * AT_vector.size() == BT_vector.size()
   *
   * @param    multipliers          the adjoint multipliers
   * @param    AT_vector            vector of transposed matrices A_1^t, ..,
   * A_N^t
   * @param    BT_vector            vector of transposed matrices B_1^t, ...,
   * B_N^t
   * @param    df_dx_vector         vector of partial derivatives f_y0, ...,
   * f_yN
   * @param    solvers              vector of Eigen solver objects
   */
  void compute_multiplier(
      std::vector<Eigen::VectorXd> &multipliers,
      std::vector<Eigen::SparseMatrix<double>> const &AT_vector,
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::VectorXd> const &df_dx_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers) {

    // first iteration j = N
    std::size_t N = multipliers.size() - 1;
    solvers[N].factorize(BT_vector[N]);
    multipliers[N] = solvers[N].solve(-1 * df_dx_vector[N]);

    for (std::size_t k = N - 1; k >= 1; --k) {
      solvers[k].factorize(BT_vector[k]);
      multipliers[k] = solvers[k].solve(
          -1 * (df_dx_vector[k] + AT_vector[k + 1] * multipliers[k + 1]));
    }
    // last iteration, j = 0
    multipliers[0] = -1.0 * (df_dx_vector[0] + AT_vector[1] * multipliers[1]);
  }
} // namespace optimization
