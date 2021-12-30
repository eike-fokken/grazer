#pragma once
#include <Eigen/Sparse>
#include <IpTypes.hpp>

namespace Model {
  class Timeevolver;
  class OptimizableObject;
} // namespace Model

namespace Optimization {
  using RowMat
      = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

  template <typename Dense, typename Scalar, int Options, typename StorageIndex>
  void assign_sparse_to_sparse_dense(
      Eigen::DenseBase<Dense> &dense,
      Eigen::SparseMatrix<Scalar, Options, StorageIndex> &sparse) {
    for (int i = 0; i < sparse.outerSize(); i++)
      for (Eigen::InnerIterator it(sparse, i); it; ++it)
        dense(it.row(), it.col()) = it.value();
  }

  class Optimizer {
  public:
    virtual ~Optimizer() {}

    virtual bool supply_constraint_jacobian_indices(
        Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Rowindices,
        Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Colindices) const = 0;

    virtual Eigen::Index get_total_no_controls() const = 0;
    virtual Eigen::Index get_total_no_constraints() const = 0;
    virtual Eigen::Index get_no_nnz_in_jacobian() const = 0;

    virtual void new_x() = 0;

    virtual bool evaluate_objective(
        Eigen::Ref<Eigen::VectorXd const> const &controls, double &objective)
        = 0;
    virtual bool evaluate_constraints(
        Eigen::Ref<Eigen::VectorXd const> const &controls,
        Eigen::Ref<Eigen::VectorXd> constraints)
        = 0;
    virtual bool evaluate_objective_gradient(
        Eigen::Ref<Eigen::VectorXd const> const &controls,
        Eigen::Ref<Eigen::VectorXd> gradient)
        = 0;
    virtual bool evaluate_constraint_jacobian(
        Eigen::Ref<Eigen::VectorXd const> const &controls,
        Eigen::Ref<Eigen::VectorXd> values)
        = 0;

    // initial values:
    virtual Eigen::VectorXd get_initial_controls() = 0;
    virtual Eigen::VectorXd get_lower_bounds() = 0;
    virtual Eigen::VectorXd get_upper_bounds() = 0;
    virtual Eigen::VectorXd get_constraint_lower_bounds() = 0;
    virtual Eigen::VectorXd get_constraint_upper_bounds() = 0;
  };

} // namespace Optimization
