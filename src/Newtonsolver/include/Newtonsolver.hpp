#pragma once
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

namespace Aux {
  class Matrixhandler;
}

/// \brief This namespace holds tools for solving numerical problems, e.g.
/// finding the root of a non-linear function.
namespace Solver {

  /// \brief The function, whose zero we are searching. Its signature is that of
  /// the evaluate methods of our model classes.
  typedef void(rootfunction)(Eigen::VectorXd &rootvalue, double last_time,
                             double new_time, Eigen::VectorXd const &last_state,
                             Eigen::VectorXd const &new_state);

  /// \brief The derivative of the root function.
  typedef void(derivative)(Aux::Matrixhandler *jacobianhandler,
                           double last_time, double new_time,
                           Eigen::VectorXd const &last_state,
                           Eigen::VectorXd const &new_state);

  /// \brief This struct holds info on the solution of a solve-execution.
  ///
  /// @param success is true, if solve found a solution.
  /// @param residual is the absolute value of f(new_state) after solve.
  /// #param the number of Newton steps need.
  struct Solutionstruct {
    bool success{false};
    double residual{std::numeric_limits<double>::quiet_NaN()};
    int used_iterations{0};
  };

  /// \brief Manages solving non-linear systems and (to be implemented)
  ///        computing derivatives with respect to controls.
  ///
  /// This class holds a SparseMatrix and a corresponding Sparse-matrix solver,
  /// so it can compute the solution of a non-linear problem.
  class Newtonsolver {
  public:
    Newtonsolver(double _tolerance, int _maximal_iterations)
        : tolerance(_tolerance), maximal_iterations(_maximal_iterations){};

    void evaluate_state_derivative_triplets(derivative df, double last_time,
                                            double new_time,
                                            Eigen::VectorXd const &last_state,
                                            Eigen::VectorXd const &new_state);

    void evaluate_state_derivative_coeffref(derivative df, double last_time,
                                            double new_time,
                                            Eigen::VectorXd const &last_state,
                                            Eigen::VectorXd const &new_state);

    /// \brief This method computes a solution to f(new_state) == 0.
    ///
    ///
    /// If new_jacobian_structure is true, it first completely rebuilds the
    /// jacobian and re-analyzes the sparsity pattern for the lu-solver. It uses
    /// an affine-invariant Newton solution described in chapter 4.2 in
    /// "Deuflhard and Hohmann: Numerical Analysis in Modern Scientific
    /// Computing". Afterwards there should hold f(new_state) == 0.
    Solutionstruct solve(rootfunction f, derivative df,
                         bool new_jacobian_structure,
                         Eigen::VectorXd &new_state, double last_time,
                         double new_time, Eigen::VectorXd const &last_state);

  private:
    /// Holds an instance of the actual solver, to save computation time it is
    /// kept from previous time steps because usually the sparsity pattern
    /// will not change.
    Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>
        sparselusolver;
    /// This must be revisited when including on the fly refinement of meshes.

    /// This will be the jacobian matrix.  We hold it here so its sparsity
    /// pattern is preserved.
    Eigen::SparseMatrix<double> jacobian;

    /// Tolerance under which equality is accepted.
    double tolerance;

    /// highest number of iterations after which to throw an exception
    int maximal_iterations;

    /// technical constant of the solve algorithm.
    constexpr static double const decrease_value{0.01};
    /// The minimal stepsize of a Newton step.
    constexpr static double const minimal_stepsize{1e-10};
  };

} // namespace Solver
