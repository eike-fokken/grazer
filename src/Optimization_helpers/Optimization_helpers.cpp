#include "Optimization_helpers.hpp"
#include "Exception.hpp"
#include <cassert>

namespace optimization {

  void initialize_solvers(
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers) {
    assert(BT_vector.size() == solvers.size());
    for (std::size_t index = 0; index != BT_vector.size(); ++index) {
      solvers[index].analyzePattern(BT_vector[index]);
    }
  }

  void compute_multiplier(
      std::vector<Eigen::VectorXd> &multipliers,
      std::vector<Eigen::SparseMatrix<double>> const &AT_vector,
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::VectorXd> const &df_dx_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers) {

    // Use solver[index].factorize(BT_vector[index]) here and then
    // solver[index].solve( .... )
  }
} // namespace optimization
