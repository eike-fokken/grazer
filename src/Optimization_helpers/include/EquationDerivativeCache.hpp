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

  class EquationDerivativeCache final {
  public:
    EquationDerivativeCache(Eigen::Index number_of_states);

    std::tuple<SolverVector const &, MatrixVector const &, MatrixVector const &>
    compute_derivatives(
        Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state,
        Model::Controlcomponent &problem);

  private:
    void initialize_derivatives(
        Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states,
        Model::Controlcomponent &problem);

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
