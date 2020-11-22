#pragma once
#include "../../testingWithGoogle/TestProblem.hpp"
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>
#include <Matrixhandler.hpp>
#include <Problem.hpp>
#include <iostream>

namespace Model {}

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
                                            Eigen::VectorXd const &last_state,
                                            Eigen::VectorXd &new_state) {
      new_state = last_state;
      {
        jacobian.resize(new_state.size(), new_state.size());
        Aux::Triplethandler handler(&jacobian);

        Aux::Triplethandler *const handler_ptr = &handler;
        problem.evaluate_state_derivative(handler_ptr, last_time, new_time,
                                          last_state, new_state);
        handler.set_matrix();
      }
      sparselusolver.analyzePattern(jacobian);
    };

    void evaluate_state_derivative_coeffref(Problemtype &problem,
                                            double last_time, double new_time,
                                            Eigen::VectorXd const &last_state,
                                            Eigen::VectorXd const &new_state) {
      Aux::Coeffrefhandler handler(&jacobian);
      Aux::Coeffrefhandler *const handler_ptr = &handler;
      problem.evaluate_state_derivative(handler_ptr, last_time, new_time,
                                        last_state, new_state);
    };

    /// \brief This method computes a solution to f(new_state) == 0.
    ///
    ///
    /// If new_jacobian_structure is true, it first completely rebuilds the
    /// jacobian and re-analyzes the sparsity pattern for the lu-solver. It uses
    /// an affine-invariant Newton solution described in chapter 4.2 in
    /// "Deuflhard and Hohmann: Numerical Analysis in Modern Scientific
    /// Computing". Afterwards there should hold f(new_state) == 0 (up to
    /// tolerance).
    Solutionstruct solve(Eigen::VectorXd &new_state, Problemtype &problem,
                         bool new_jacobian_structure, double last_time,
                         double new_time, Eigen::VectorXd const &last_state) {
      Solutionstruct solstruct;

      // If the structure of the jacobian changed one should generate it from
      // triplets anew:
      if (new_jacobian_structure) {
        evaluate_state_derivative_triplets(problem, last_time, new_time,
                                           last_state, new_state);
      }
      // If the non-zero elements are the same, just update them:
      else {
        evaluate_state_derivative_coeffref(problem, last_time, new_time,
                                           last_state, new_state);
      }

      Eigen::VectorXd temp1(new_state.size());
      Eigen::VectorXd temp2(new_state.size());
      Eigen::VectorXd *vec1 = &temp1;
      Eigen::VectorXd *vec2 = &temp2;
      Eigen::VectorXd *temp;

      Eigen::VectorXd function(new_state.size());
      Eigen::VectorXd step(new_state.size());
      double stepsizeparameter;
      double oldstepsizeparameter;
      double stepsize;

      // set initial guess to new_state:
      *vec1 = new_state;
      *vec2 = new_state;
      problem.evaluate(function, last_time, new_time, last_state, *vec1);
      solstruct.residual = function.lpNorm<Eigen::Infinity>();

      while (solstruct.residual > tolerance &&
             solstruct.used_iterations < maximal_iterations) {

        problem.evaluate(function, last_time, new_time, last_state, *vec1);
        evaluate_state_derivative_triplets(problem, last_time, new_time,
                                           last_state, *vec1);
        sparselusolver.compute(jacobian);
        step = sparselusolver.solve(-function);
        oldstepsizeparameter = step.lpNorm<2>();
        stepsize = 2.0; // This is set to 2.0, so that in the first run of the
                        // following loop the stepsize is decreased to the
                        // correct initial value of 1.0.

        // Stepsize computation:
        do {
          stepsize *= 0.5;
          *vec2 = *vec1 + stepsize * step;
          problem.evaluate(function, last_time, new_time, last_state, *vec2);
          stepsizeparameter =
              sparselusolver.solve(-function).template lpNorm<2>();
          // std::cout << stepsizeparameter << " ";
        } while ((stepsizeparameter >
                  (1 - decrease_value * stepsize) * oldstepsizeparameter) &&
                 stepsize > minimal_stepsize);

        solstruct.residual = function.lpNorm<Eigen::Infinity>();
        temp = vec1;
        vec1 = vec2; // we switch pointers in order to not copy data and still
                     // have access to the last value.
        vec2 = temp;
        ++solstruct.used_iterations;
      }
      new_state = *vec1;

      if (solstruct.used_iterations == maximal_iterations) {
        solstruct.success = false;
      } else {
        solstruct.success = true;
      }

      return solstruct;
    };

  private:
    /// Holds an instance of the actual solver, to save computation time it
    /// is kept from previous time steps because usually the sparsity
    /// pattern will not change.
    Eigen::SparseLU<Eigen::SparseMatrix<double>, Eigen::COLAMDOrdering<int>>
        sparselusolver;
    /// This must be revisited when including on the fly refinement of
    /// meshes.

    /// This will be the jacobian matrix.  We hold it here so its sparsity
    /// pattern is preserved.
    Eigen::SparseMatrix<double> jacobian;

    /// Tolerance under which equality is accepted.
    double tolerance;

    /// highest number of iterations after which to throw an exception
    int maximal_iterations;

    /// technical constant of the solve algorithm.
    constexpr static double const decrease_value{0.001};
    /// The minimal stepsize of a Newton step.
    constexpr static double const minimal_stepsize{1e-10};
  };
  typedef Newtonsolver_temp<Model::Problem> Newtonsolver;
  typedef Newtonsolver_temp<TestProblem> Newtonsolver_test;
} // namespace Solver
