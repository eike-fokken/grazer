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
#include "Cacheentry.hpp"
#include "InterpolatingVector.hpp"
#include "OptimizableObject.hpp"
#include <Eigen/src/SparseCore/SparseMatrix.h>
#include <nlohmann/json.hpp>

namespace Model {
  class Controlcomponent;
} // namespace Model

namespace Optimization {
  using SolverVector
      = std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>>;
  using MatrixVector = std::vector<Eigen::SparseMatrix<double>>;

  /** @brief A cache for the timestep-wise derivatives of the equations w.r.t.
   * states and controls.
   *
   * This class hands out (references to) vectors of derivative matrices of the
   * equations. In case the input arguments (controls, timepoints where the
   * equations are evaluated and initial state) change, a new set of vectors is
   * computed. Otherwise the precomputed cache is supplied.
   */
  class EquationDerivativeCache final {
  public:
    /** @brief The constructor needs the number of states because the vector
     * #dE_dnew_state_solvers must default construct all its members, as they
     * are immovable.
     */
    EquationDerivativeCache(Eigen::Index number_of_states);

    /** @brief Supplies a reference to the derivatives
     *
     * @param controls The controls for evaluation of the equations.
     * @param states The states for evaluation of the equations.
     * @param problem The problem whose equation derivatives should be
     * evaluated.
     * @returns tuple of vectors of (LU solvers for) dE_dnewstate,
     * dE_dlast_state, dE_dcontrol
     */
    std::tuple<SolverVector const &, MatrixVector const &, MatrixVector const &>
    compute_derivatives(
        Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states,
        Model::Controlcomponent &problem);

  private:
    /** @brief Initializes and evaluates the derivatives.
     */
    void initialize_derivatives(
        Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states,
        Model::Controlcomponent &problem);

    /** @brief evaluates the derivatives. Is only valid to run if
     * initialize_derivatives has been run before.
     */
    void update_derivatives(
        Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states,
        Model::Controlcomponent &problem);

    /** \brief The cache entry itself, consisting of controls, state timesteps
     * and the initial state.
     */
    Cacheentry entry;
    /** \brief A vector of solvers, one for each timestep for the matrix
     * dE_dnew_state.
     */
    std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>>
        dE_dnew_state_solvers;
    /** \brief A cache matrix. Used to prevent reallocation of a sparse matrix
     * of equal sparsity pattern.
     */
    Eigen::SparseMatrix<double> dE_dnew_matrix;
    /** \brief A vector of the matrices dE_dlast_state for each timestep.
     */
    std::vector<Eigen::SparseMatrix<double>> dE_dlast_state;

    /** \brief A vector of the matrices dE_dcontrol for each timestep.
     */
    std::vector<Eigen::SparseMatrix<double>> dE_dcontrol;

    /** \brief Tracks whether the cache values have already been initialized.
     */
    bool initialized = false;
  };

} // namespace Optimization
