#include "Optimization_helpers.hpp"

namespace optimization {

  void compute_multiplier(
      std::vector<Eigen::VectorXd> &multipliers,
      std::vector<Eigen::SparseMatrix<double>> const &AT_vector,
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::VectorXd> const &df_dx_vector) {
    Eigen::VectorXd a;
    a << 0., 0., 0., 0., 0., 0., 0., 0.;
    for (auto &multiplier : multipliers) { multiplier = a; }
  }
} // namespace optimization
