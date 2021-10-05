#pragma once
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>

/// \brief This namespace holds tools for solving numerical problems, e.g.
/// finding the root of a non-linear function.
namespace Solver {

  /// \brief This struct holds info on the solution of a solve-execution.
  ///
  /// @param success is true, if solve found a solution.
  /// @param residual is the absolute value of f(new_state) after solve.
  /// #param the number of Newton steps need.
  struct Solutionstruct {
    bool success{false};
    double residual{1000000.0};
    int used_iterations{0};
  };

  /// \brief Manages solving non-linear systems and (to be implemented)
  ///        computing derivatives with respect to controls.
  ///
  /// This class holds a SparseMatrix and a corresponding Sparse-matrix solver,
  /// so it can compute the solution of a non-linear problem.
  template <typename Problemtype> class Newtonsolver {
  public:
    Newtonsolver(double _tolerance, int _maximal_iterations);

    /// \brief Reanalyzes the sparsity pattern of the jacobian the objective
    /// function and computes it.
    ///
    /// The jacobian is saved into the data member named "jacobian".

    void evaluate_state_derivative_triplets(
        Problemtype &problem, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control);

    /// \brief Computes the jacobian with the assumption that the sparsity
    /// pattern has not changed.
    ///
    /// The jacobian is saved into the data member named "jacobian".
    /// Only call this version if you are sure that the sparsity pattern is
    /// unchanged.

    void evaluate_state_derivative_coeffref(
        Problemtype &problem, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control);

    /// \brief Returns the number of structurally non-zero indices of the
    /// jacobian.
    Eigen::Index get_number_non_zeros_jacobian();

    /// \brief This method computes a solution to f(new_state) == 0.
    ///
    /// It uses
    /// an affine-invariant Newton method described in chapter 4.2 in
    /// "Deuflhard and Hohmann: Numerical Analysis in Modern Scientific
    /// Computing". Afterwards there should hold f(new_state) == 0 (up to
    /// tolerance).
    Solutionstruct solve(
        Eigen::Ref<Eigen::VectorXd> new_state, Problemtype &problem,
        bool newjac, bool use_full_jacobian, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &control);

  private:
    /// Holds an instance of the actual solver, to save computation time it
    /// is kept from previous time steps because usually the sparsity
    /// pattern will not change.
    Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>
        lusolver;

    // Later on we will include qr decomposition for badly conditioned
    // jacobians. Eigen::SparseQR<Eigen::SparseMatrix<double>,
    // Eigen::COLAMDOrdering<int>> qrsolver;

    /// This will be the jacobian matrix.  We hold it here so its sparsity
    /// pattern is preserved.
    Eigen::SparseMatrix<double> jacobian;

    /// Tolerance under which equality is accepted.
    double tolerance;

    /// highest number of iterations after which to give up.
    int maximal_iterations;

    /// technical constant of the solve algorithm.
    constexpr static double const decrease_value{1e-3};
    /// The minimal stepsize of a Newton step.
    constexpr static double const minimal_stepsize{1e-12};
  };

} // namespace Solver
