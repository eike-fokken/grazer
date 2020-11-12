#pragma once
#include <Eigen/Sparse>
#include <Eigen/SparseLU>

namespace Aux {
  class Matrixhandler;
}

namespace Solver {

  typedef void(rootfunction)(Eigen::VectorXd &rootfunction, double last_time,
                             double new_time, Eigen::VectorXd const &last_state,
                             Eigen::VectorXd const &new_state);

  typedef void(derivative)(Aux::Matrixhandler *jacobianhandler,
                           double last_time, double new_time,
                           Eigen::VectorXd const &last_state,
                           Eigen::VectorXd const &new_state);

  struct Solutionstruct {
    bool success{false};
    double residual{std::numeric_limits<double>::quiet_NaN()};
    int used_iterations{0};
  };

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

    constexpr static double const decrease_value{0.01};
    constexpr static double const minimal_stepsize{1e-10};
  };

} // namespace Solver
