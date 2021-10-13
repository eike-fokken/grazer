#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
#include <vector>

namespace Aux {
  class Controller;
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
      std::vector<Eigen::VectorXd> &multipliers,
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
      Model::Timedata timedata, Model::Networkproblem &problem,
      Aux::Controller &controller, nlohmann::json const &control_json,
      Eigen::Ref<Eigen::VectorXd> init_state,
      nlohmann::json const &initial_json,
      Eigen::Ref<Eigen::VectorXd> lower_bounds,
      nlohmann::json const &lower_bounds_json,
      Eigen::Ref<Eigen::VectorXd> upper_bounds,
      nlohmann::json const &upper_bounds_json,
      Eigen::Ref<Eigen::VectorXd> constraints_lower_bounds,
      nlohmann::json const &constraints_lower_bounds_json,
      Eigen::Ref<Eigen::VectorXd> constraints_upper_bounds,
      nlohmann::json const &constraints_upper_bounds_json);

} // namespace Optimization
