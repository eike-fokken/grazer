/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#pragma once
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/SparseQR>
#include <stdexcept>

/// \brief This namespace holds tools for solving numerical problems, e.g.
/// finding the root of a non-linear function.
namespace Model {
  class Controlcomponent;
}
namespace Solver {

  class SolverNumericalProblem : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  /** \brief This struct holds info on the solution of a solve-execution.
   */
  struct Solutionstruct {
    bool success{false}; //! is true, if solve found a solution.
    double residual{
        1000000.0}; //! is the absolute value of f(new_state) after solve.
    int used_iterations{0}; //! the number of needed Newton steps
    /** \brief the number of Newton steps needed.
     */
  };

  /** \brief Manages solving non-linear systems and (to be implemented)
   *        computing derivatives with respect to controls.
   *
   * This class holds a SparseMatrix and a corresponding Sparse-matrix solver,
   * so it can compute the solution of a non-linear problem.
   */
  class Newtonsolver {
  public:
    Newtonsolver(double _tolerance, int _maximal_iterations);

    /** \brief Reanalyzes the sparsity pattern of the jacobian the objective
     * function and computes it.
     *
     * The jacobian is saved into the data member named "jacobian".
     */
    void evaluate_state_derivative_triplets(
        Model::Controlcomponent const &problem, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control);

    /** \brief Computes the jacobian with the assumption that the sparsity
     * pattern has not changed.
     *
     * The jacobian is saved into the data member named "jacobian".
     * Only call this version if you are sure that the sparsity pattern is
     * unchanged.
     */

    void evaluate_state_derivative_coeffref(
        Model::Controlcomponent const &problem, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control);

    /** \brief Returns the number of structurally non-zero indices of the
     * jacobian.
     */
    Eigen::Index get_number_non_zeros_jacobian();

    /** \brief Returns n, where the jacobian is a matrix of dimension n x n.
     */
    Eigen::Index get_dimension_of_jacobian();

    /** \brief This method computes a solution to f(new_state) == 0.
     *
     * It uses
     * an affine-invariant Newton method described in chapter 4.2 in
     * "Deuflhard and Hohmann: Numerical Analysis in Modern Scientific
     * Computing". Afterwards there should hold f(new_state) == 0 (up to
     * tolerance).
     */
    Solutionstruct solve(
        Eigen::Ref<Eigen::VectorXd> new_state,
        Model::Controlcomponent const &problem, bool newjac,
        bool use_full_jacobian, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &control);

  private:
    /** Holds an instance of the actual solver, to save computation time it
     * is kept from previous time steps because usually the sparsity
     * pattern will not change.
     */
    Eigen::SparseLU<Eigen::SparseMatrix<double>> lusolver;

    // Later on we may include qr decomposition for badly conditioned
    // jacobians. Eigen::SparseQR<Eigen::SparseMatrix<double>,
    // Eigen::COLAMDOrdering<int>> qrsolver;

    /** This will be the jacobian matrix.  We hold it here so its sparsity
     * pattern is preserved.
     */
    Eigen::SparseMatrix<double> jacobian;

    /** Tolerance under which equality is accepted.
     */
    double tolerance;

    /** highest number of iterations after which to give up.
     */
    int maximal_iterations;

    /** technical constant of the solve algorithm.
     */
    constexpr static double const decrease_value{1e-3};
    /** The minimal stepsize of a Newton step.
     */
    constexpr static double const minimal_stepsize{1e-12};
  };

} // namespace Solver
