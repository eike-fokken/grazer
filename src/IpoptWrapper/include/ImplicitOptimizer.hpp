#pragma once
#include "ConstraintJacobian.hpp"
#include "InterpolatingVector.hpp"
#include "Optimizer.hpp"
#include <memory>

namespace Model {
  class OptimizableObject;
} // namespace Model

namespace Optimization {
  struct Initialvalues;
  class StateCache;

  class ImplicitOptimizer final : public Optimizer {
  public:
    ImplicitOptimizer(
        std::unique_ptr<Model::OptimizableObject> problem,
        std::unique_ptr<StateCache> cache,
        Eigen::Ref<Eigen::VectorXd const> const &state_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &control_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &constraint_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state,
        Aux::InterpolatingVector_Base const &initial_controls,
        Aux::InterpolatingVector_Base const &lower_bounds,
        Aux::InterpolatingVector_Base const &upper_bounds,
        Aux::InterpolatingVector_Base const &constraint_lower_bounds,
        Aux::InterpolatingVector_Base const &constraint_upper_bounds);

    ~ImplicitOptimizer() final;

    bool supply_constraint_jacobian_indices(
        Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Rowindices,
        Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Colindices) const final;

    Eigen::Index get_total_no_controls() const final;
    Eigen::Index get_total_no_constraints() const final;
    Eigen::Index get_no_nnz_in_jacobian() const final;

    void new_x() final;

    std::tuple<bool, bool, bool> get_boolians() const;

    bool evaluate_objective(
        Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
        double &objective) final;
    bool evaluate_constraints(
        Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
        Eigen::Ref<Eigen::VectorXd> ipoptconstraints) final;
    bool evaluate_objective_gradient(
        Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
        Eigen::Ref<Eigen::VectorXd> gradient) final;
    bool evaluate_constraint_jacobian(
        Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
        Eigen::Ref<Eigen::VectorXd> values) final;

    // initial values:
    Eigen::Ref<Eigen::VectorXd const> get_initial_state();
    Eigen::VectorXd get_initial_controls() final;
    Eigen::VectorXd get_lower_bounds() final;
    Eigen::VectorXd get_upper_bounds() final;
    Eigen::VectorXd get_constraint_lower_bounds() final;
    Eigen::VectorXd get_constraint_upper_bounds() final;

    bool compute_derivatives(
        Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states);

    /** \brief Allocates room for the structures of the matrices
     * #dE_dnew_transposed #dE_dlast_transposed, #dE_dcontrol,
     * #dg_dnew_transposed, #dg_dcontrol and anlyzes the pattern of
     * #dE_dnew_transposed in the #solver member variable.
     */
    void initialize_derivative_matrices(
        Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states);

    /** \brief fills the matrices #dE_dnew_transposed #dE_dlast_transposed and
     * #dE_dcontrol with their values at state_index and fills #solver with the
     * factorization of #dE_dnew_transposed.
     */
    void update_equation_derivative_matrices(
        Eigen::Index state_index, Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states);

    void update_constraint_derivative_matrices(
        Eigen::Index state_index, Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states);

    void update_cost_derivative_matrices(
        Eigen::Index state_index, Aux::InterpolatingVector_Base const &controls,
        Aux::InterpolatingVector_Base const &states);

    // getters:
    Aux::InterpolatingVector_Base const &get_current_full_state() const;
    Aux::InterpolatingVector_Base const &get_objective_gradient() const;
    ConstraintJacobian_Base const &get_constraint_jacobian() const;

    // convenience methods:
    Eigen::Index states_per_step() const;
    Eigen::Index controls_per_step() const;
    Eigen::Index constraints_per_step() const;

    /** \brief returns the number of compute steps for the states. This
    excludes
     * the initial state timepoint, because there only the initial
     conditions
     * are present.
     */
    Eigen::Index state_steps() const;
    Eigen::Index control_steps() const;
    Eigen::Index constraint_steps() const;

    // Matrix block methods:

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

  private:
    // members:
    std::unique_ptr<Model::OptimizableObject>
        problem; // Order dependency before
    std::unique_ptr<Initialvalues> init;
    std::unique_ptr<StateCache> cache;
    bool derivative_matrices_initialized = false;
    bool states_up_to_date = false;
    bool derivatives_up_to_date = false;
    Eigen::VectorXd const state_timepoints;      // Order dependency before
    Eigen::VectorXd const control_timepoints;    // Order dependency before
    Eigen::VectorXd const constraint_timepoints; // Order dependency before
    Eigen::VectorXd const initial_state;
    Eigen::VectorX<std::pair<Eigen::Index, double>> const
        index_lambda_pairs; // Order dependency after timepoints.
    Aux::InterpolatingVector
        objective_gradient; // Order dependency after (problem and timepoints)
    ConstraintJacobian constraint_jacobian; // Order dependency before () and
                                            // after (problem and timepoints)
    MappedConstraintJacobian
        constraintjacobian_accessor; // Order dependency (after
                                     // constraint_jacobian)
    Eigen::SparseMatrix<double> dE_dnew_transposed;
    Eigen::SparseMatrix<double> dE_dlast_transposed;
    Eigen::SparseMatrix<double> dE_dcontrol;
    Eigen::SparseMatrix<double> df_dnew_transposed;
    Eigen::SparseMatrix<double> df_dcontrol;
    Eigen::SparseMatrix<double> dg_dnew_transposed;
    Eigen::SparseMatrix<double> dg_dcontrol;

    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

    // cache_matrices :
    // Eigen::MatrixXd A_jp1_Lambda_j;
    // Eigen::MatrixXd Lambda_j;
    // RowMat dg_duj;
  };
} // namespace Optimization
