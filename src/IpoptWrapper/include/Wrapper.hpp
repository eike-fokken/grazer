#pragma once

#include "ConstraintJacobian.hpp"
#include "ControlStateCache.hpp"
#include "InterpolatingVector.hpp"

#include <Eigen/Sparse>
#include <Eigen/src/Core/util/Constants.h>
#include <limits>
#include <memory>
#include <nlohmann/json.hpp>

#include <IpTNLP.hpp>

using RowMat
    = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

template <typename Dense, typename Scalar, int Options, typename StorageIndex>
void assign_sparse_to_sparse_dense(
    Eigen::DenseBase<Dense> &dense,
    Eigen::SparseMatrix<Scalar, Options, StorageIndex> &sparse) {
  for (int i = 0; i < sparse.outerSize(); i++)
    for (Eigen::InnerIterator it(sparse, i); it; ++it)
      dense(it.row(), it.col()) = it.value();
}

namespace Aux {
  class InterpolatingVector_Base;
}

namespace Model {
  class Timeevolver;
  class OptimizableObject;
} // namespace Model

namespace Optimization {

  struct Initialvalues {
    Initialvalues(
        Aux::InterpolatingVector initial_controls,
        Aux::InterpolatingVector lower_bounds,
        Aux::InterpolatingVector upper_bounds,
        Aux::InterpolatingVector constraint_lower_bounds,
        Aux::InterpolatingVector constraint_upper_bounds);
    Aux::InterpolatingVector const initial_controls;
    Aux::InterpolatingVector const lower_bounds;
    Aux::InterpolatingVector const upper_bounds;
    Aux::InterpolatingVector const constraint_lower_bounds;
    Aux::InterpolatingVector const constraint_upper_bounds;

    bool obsolete() const;

    bool called_get_nlp_info = false;
    bool called_get_bounds_info = false;
    bool called_get_starting_point = false;
  };

  class IpoptWrapper final : public Ipopt::TNLP {

  public:
    IpoptWrapper(
        Model::Timeevolver &evolver, Model::OptimizableObject &problem,
        Eigen::VectorXd _state_timepoints, Eigen::VectorXd _control_timepoints,
        Eigen::VectorXd _constraint_timepoints, Eigen::VectorXd initial_state,
        Aux::InterpolatingVector initial_controls,
        Aux::InterpolatingVector lower_bounds,
        Aux::InterpolatingVector upper_bounds,
        Aux::InterpolatingVector constraint_lower_bounds,
        Aux::InterpolatingVector constraint_upper_bounds);

    ~IpoptWrapper() = default;

    Aux::InterpolatingVector_Base const &get_solution() const;
    double get_final_objective_value() const;

    // Internal Ipopt methods

    /** Method to return some info about the nlp */
    bool get_nlp_info(
        Ipopt::Index &n, Ipopt::Index &m, Ipopt::Index &nnz_jac_g,
        Ipopt::Index &nnz_h_lag, IndexStyleEnum &index_style) final;

    /** Method to return the bounds for my problem */
    bool get_bounds_info(
        Ipopt::Index n, Ipopt::Number *x_l, Ipopt::Number *x_u, Ipopt::Index m,
        Ipopt::Number *g_l, Ipopt::Number *g_u) final;

    /** Method to return the starting point for the algorithm */
    bool get_starting_point(
        Ipopt::Index n, bool init_x, Ipopt::Number *x, bool init_z,
        Ipopt::Number *z_L, Ipopt::Number *z_U, Ipopt::Index m,
        bool init_lambda, Ipopt::Number *lambda) final;

    /** Method to return the objective value */
    bool eval_f(
        Ipopt::Index n, const Ipopt::Number *x, bool new_x,
        Ipopt::Number &obj_value) final;

    /** Method to return the gradient of the objective */
    bool eval_grad_f(
        Ipopt::Index n, const Ipopt::Number *x, bool new_x,
        Ipopt::Number *grad_f) final;

    /** Method to return the constraint residuals */
    bool eval_g(
        Ipopt::Index n, const Ipopt::Number *x, bool new_x, Ipopt::Index m,
        Ipopt::Number *g) final;

    /** Method to return:
     *   1) The structure of the Jacobian (if "values" is NULL)
     *   2) The values of the Jacobian (if "values" is not NULL)
     */
    bool eval_jac_g(
        Ipopt::Index n, const Ipopt::Number *x, bool new_x, Ipopt::Index m,
        Ipopt::Index nele_jac, Ipopt::Index *iRow, Ipopt::Index *jCol,
        Ipopt::Number *values) final;

    /** This method is called when the algorithm is complete so the TNLP can
     * store/write the solution
     */
    void finalize_solution(
        Ipopt::SolverReturn status, Ipopt::Index n, const Ipopt::Number *x,
        const Ipopt::Number *z_L, const Ipopt::Number *z_U, Ipopt::Index m,
        const Ipopt::Number *g, const Ipopt::Number *lambda,
        Ipopt::Number obj_value, const Ipopt::IpoptData *ip_data,
        Ipopt::IpoptCalculatedQuantities *ip_cq) final;

  private:
    bool compute_derivatives(bool new_x, Ipopt::Number const *x);

    /** \brief Allocates room for the structures of the matrices
     * #dE_dnew_transposed #dE_dlast_transposed, #dE_dcontrol,
     * #dg_dnew_transposed, #dg_dcontrol and anlyzes the pattern of
     * #dE_dnew_transposed in the #solver member variable.
     */
    void initialize_derivative_matrices(
        Aux::InterpolatingVector_Base const &states,
        Aux::InterpolatingVector_Base const &controls);

    /** \brief fills the matrices #dE_dnew_transposed #dE_dlast_transposed and
     * #dE_dcontrol with their values at state_index and fills #solver with the
     * factorization of #dE_dnew_transposed.
     */
    void update_equation_derivative_matrices(
        Eigen::Index state_index, Aux::InterpolatingVector_Base const &states,
        Aux::InterpolatingVector_Base const &controls);

    void update_constraint_derivative_matrices(
        Eigen::Index state_index, Aux::InterpolatingVector_Base const &states,
        Aux::InterpolatingVector_Base const &controls);

    void update_cost_derivative_matrices(
        Eigen::Index state_index, Aux::InterpolatingVector_Base const &states,
        Aux::InterpolatingVector_Base const &controls);

    Aux::InterpolatingVector objective_gradient;
    ConstraintJacobian constraint_jacobian;
    MappedConstraintJacobian constraint_jacobian_accessor;

    // cache_matrices :
    Eigen::MatrixXd A_jp1_Lambda_j;
    Eigen::MatrixXd Lambda_j;
    RowMat dg_duj;

    Model::OptimizableObject &problem;
    ControlStateCache cache;

    Eigen::SparseMatrix<double> dE_dnew_transposed;
    Eigen::SparseMatrix<double> dE_dlast_transposed;
    Eigen::SparseMatrix<double> dE_dcontrol;
    Eigen::SparseMatrix<double> df_dnew_transposed;
    Eigen::SparseMatrix<double> df_dcontrol;
    Eigen::SparseMatrix<double> dg_dnew_transposed;
    Eigen::SparseMatrix<double> dg_dcontrol;

    Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
        dg_dnew_dense_transposed;

    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    Eigen::VectorXd const state_timepoints;
    Eigen::VectorXd const control_timepoints;
    Eigen::VectorXd const constraint_timepoints;
    Eigen::VectorXd const initial_state;
    std::unique_ptr<Initialvalues> init;
    Aux::InterpolatingVector solution;
    bool current_derivatives_computed = false;
    bool derivative_matrices_initialized = false;

    double final_objective_value{std::numeric_limits<double>::max()};

    /** \brief fills in the value vector for the constraints.
     */
    bool evaluate_constraints(
        Ipopt::Number const *x, Ipopt::Index number_of_controls,
        Ipopt::Number *values, Ipopt::Index nele_jac);

  public:
    Eigen::Index states_per_step() const;
    Eigen::Index controls_per_step() const;
    Eigen::Index constraints_per_step() const;

    /** \brief returns the number of compute steps for the states. This excludes
     * the initial state timepoint, because there only the initial conditions
     * are present.
     */
    Eigen::Index state_steps() const;
    Eigen::Index control_steps() const;
    Eigen::Index constraint_steps() const;

    Eigen::Index get_total_no_controls() const;
    Eigen::Index get_total_no_constraints() const;

    Eigen::Ref<Eigen::MatrixXd> right_cols(
        Eigen::Ref<Eigen::MatrixXd> Fullmat,
        Eigen::Index outer_col_index) const;

    Eigen::Ref<Eigen::MatrixXd> middle_col_block(
        Eigen::Ref<Eigen::MatrixXd> Fullmat,
        Eigen::Index outer_col_index) const;

    Eigen::Ref<RowMat>
    lower_rows(Eigen::Ref<RowMat> Fullmat, Eigen::Index outer_col_index) const;

    Eigen::Ref<RowMat> middle_row_block(
        Eigen::Ref<RowMat> Fullmat, Eigen::Index outer_col_index) const;
  };

} // namespace Optimization
