#include "Constraintcomponent.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/src/SparseCore/SparseMatrix.h>
#include <nlohmann/json.hpp>
#include <vector>

namespace Aux {
  class InterpolatingVector;
}

namespace Model {
  struct Timedata;
  class Timeevolver;
  class Networkproblem;
} // namespace Model

namespace Optimization {

  /** \brief initializes the solvers with the sparsity pattern of the respective
   * matrices.
   *
   * @param[in] BT_vector A vector of matrices whose sparsity pattern shall be
   * loaded into the solvers in #solvers.
   * @param[out] solvers A vector of LU solvers.
   */
  void initialize_solvers(
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers);

  /** \brief computes the Lagrange multipliers for the adjoint method.
   *
   * @param[out] multipliers The vector of Lagrange multipliers to be
   * computed
   * @param[in] AT_vector vector of matrices that are the transpose of the
   * derivative of the model equations with respect to the last state.
   * @param[in] BT_vector vector of matrices that are the transpose of the
   * derivative of the model equations with respect to the new state.
   * @param[in] df_dx_vector derivative of the objective function (or
   * constraint value) with respect to the new state.
   * @param[in] solvers A vector of solvers, that are initialized with the
   * right sparsity pattern for the corresponding matrix in #BT_vector
   */
  void compute_multiplier(
      bool already_factorized, std::vector<Eigen::VectorXd> &multipliers,
      std::vector<Eigen::SparseMatrix<double>> const &AT_vector,
      std::vector<Eigen::SparseMatrix<double>> const &BT_vector,
      std::vector<Eigen::VectorXd> const &df_dx_vector,
      std::vector<Eigen::SparseLU<Eigen::SparseMatrix<double>>> &solvers);

  /** \brief compute the states from given controls.
   */
  void compute_state_from_controls(
      Model::Timeevolver &timeevolver, Model::Networkproblem &problem,
      std::vector<Eigen::VectorXd> states,
      Eigen::Ref<Eigen::VectorXd const> const &controls);

  /** \brief Takes care of all initialization
   */
  void initialize(
      Model::Networkproblem &problem, Aux::InterpolatingVector &controls,
      nlohmann::json const &control_json,
      Eigen::Ref<Eigen::VectorXd> init_state,
      nlohmann::json const &initial_json,
      Aux::InterpolatingVector &lower_bounds,
      nlohmann::json const &lower_bounds_json,
      Aux::InterpolatingVector &upper_bounds,
      nlohmann::json const &upper_bounds_json,
      Aux::InterpolatingVector &constraints_lower_bounds,
      nlohmann::json const &constraints_lower_bounds_json,
      Aux::InterpolatingVector &constraints_upper_bounds,
      nlohmann::json const &constraints_upper_bounds_json);

  /** @brief computes the jacobian of the constraints in every time step and
   * saves them into a vector of sparse matrices.
   * @param[out] stepwise_handlers These save the jacobians into matrices.
   * @param[in] problem The problem for which the jacobians are computed.
   * @param[in] constraint_times time steps for constraint evaluation.
   * @param[in] controls Control variables.
   * @param[in] states State variables.
   */
  void d_constraints_d_constraint_time_control(
      std::vector<Aux::Matrixhandler> &stepwise_handlers,
      Model::Constraintcomponent &problem,
      std::vector<double> const &constraint_times,
      Aux::InterpolatingVector const &controls,
      Aux::InterpolatingVector const &states);

  /** @brief Computes Indices and weights for the derivative of controls on a
   *  fine time grid w.r.t. controls on a coarse time grid.
   *
   * This assumes that the controls on the fine time grid are linear
   * interpolations of the controls on the coarse grid.
   * \todo { Explain the structure in the pdf userguide. }
   * @param fine_resolution_vector Vector with finer interpoluation steps.
   * @param coarse_resolution_vector Vector with coarser interpoluation steps.
   * @returns tuple of row index, column index and value for lambda
   *
   */
  Eigen::SparseMatrix<double> compute_control_conversion(
      Aux::InterpolatingVector const &fine_resolution_vector,
      Aux::InterpolatingVector const &coarse_resolution_vector);

  /** @brief Returns a matrix that contains 1 at all points where a constraint
   * jacobian can be non-zero and zero everywhere else.
   */
  Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
  constraint_jac_structure(
      Aux::InterpolatingVector const &constraints,
      Aux::InterpolatingVector const &controls);

} // namespace Optimization
