#pragma once
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>

namespace GrazerTest {
class TestProblem;
}

namespace Model {
  class Problem;  
} // namespace Model


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
  template <typename Problemtype> class Newtonsolver_temp {
  public:
    Newtonsolver_temp(double _tolerance, int _maximal_iterations)
      : tolerance(_tolerance), maximal_iterations(_maximal_iterations){};

    void evaluate_state_derivative_triplets(Problemtype &problem,
                                            double last_time, double new_time,
                                            Eigen::Ref<Eigen::VectorXd const> const &last_state,
                                            Eigen::Ref<Eigen::VectorXd>new_state);

    void evaluate_state_derivative_coeffref(Problemtype &problem,
                                            double last_time, double new_time,
                                            Eigen::Ref<Eigen::VectorXd const> const &last_state,
                                            Eigen::Ref<Eigen::VectorXd const> const &new_state);


    long int get_number_non_zeros_jacobian();

    /// \brief This method computes a solution to f(new_state) == 0.
    ///
    /// It uses
    /// an affine-invariant Newton method described in chapter 4.2 in
    /// "Deuflhard and Hohmann: Numerical Analysis in Modern Scientific
    /// Computing". Afterwards there should hold f(new_state) == 0 (up to
    /// tolerance).

    Solutionstruct solve(Eigen::Ref<Eigen::VectorXd>new_state, Problemtype &problem,
                         bool newjac, double last_time, double new_time,
                         Eigen::Ref<Eigen::VectorXd const> const &last_state);
  private:
    /// Holds an instance of the actual solver, to save computation time it
    /// is kept from previous time steps because usually the sparsity
    /// pattern will not change.
    Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>
    lusolver;

    // Later on we will include qr decomposition for badly conditioned jacobians.
    // Eigen::SparseQR<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>
    // qrsolver;


    /// This will be the jacobian matrix.  We hold it here so its sparsity
    /// pattern is preserved.
    /// TODO: maybe unnecessary because we set from triplets in every step.
    // But maybe will change that again.
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

  extern template class Newtonsolver_temp<Model::Problem>;
  extern template class Newtonsolver_temp<GrazerTest::TestProblem>;

  typedef Newtonsolver_temp<Model::Problem> Newtonsolver;
  typedef Newtonsolver_temp<GrazerTest::TestProblem> Newtonsolver_test;
} // namespace Solver
