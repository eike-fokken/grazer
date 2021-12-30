#pragma once
#include "ConstraintJacobian.hpp"
#include "ControlStateCache.hpp"
#include "InterpolatingVector.hpp"
#include "Optimizer.hpp"

namespace Model {
  class OptimizableObject;
} // namespace Model

namespace Optimization {
  struct Initialvalues;

  class Implicit_Optimizer final : public Optimizer {
  public:
    Implicit_Optimizer(
        Model::OptimizableObject &problem, Model::Timeevolver &evolver,
        Eigen::Ref<Eigen::VectorXd const> const &state_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &control_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &constraint_timepoints,
        Eigen::Ref<Eigen::VectorXd const> const &initial_state,
        Aux::InterpolatingVector_Base const &initial_controls,
        Aux::InterpolatingVector_Base const &lower_bounds,
        Aux::InterpolatingVector_Base const &upper_bounds,
        Aux::InterpolatingVector_Base const &constraint_lower_bounds,
        Aux::InterpolatingVector_Base const &constraint_upper_bounds);

    bool supply_constraint_jacobian_indices(
        Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Rowindices,
        Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Colindices) const final;

    Eigen::Index get_total_no_controls() const final;
    Eigen::Index get_total_no_constraints() const final;
    Eigen::Index get_no_nnz_in_jacobian() const final;

    void new_x() final;

    bool evaluate_objective(
        Eigen::Ref<Eigen::VectorXd const> const &controls,
        double &objective) final;
    bool evaluate_constraints(
        Eigen::Ref<Eigen::VectorXd const> const &controls,
        Eigen::Ref<Eigen::VectorXd> constraints) final;
    bool evaluate_objective_gradient(
        Eigen::Ref<Eigen::VectorXd const> const &controls,
        Eigen::Ref<Eigen::VectorXd> gradient) final;
    bool evaluate_constraint_jacobian(
        Eigen::Ref<Eigen::VectorXd const> const &controls,
        Eigen::Ref<Eigen::VectorXd> values) final;

    // initial values:
    Eigen::VectorXd get_initial_controls() final;
    Eigen::VectorXd get_lower_bounds() final;
    Eigen::VectorXd get_upper_bounds() final;
    Eigen::VectorXd get_constraint_lower_bounds() final;
    Eigen::VectorXd get_constraint_upper_bounds() final;

  private:
    Model::OptimizableObject &problem; // Order dependency before
    std::unique_ptr<Initialvalues> init;
    ControlStateCache cache;
    bool states_up_to_date = false;
    bool derivatives_up_to_date = false;
    Eigen::VectorXd const state_timepoints;      // Order dependency before
    Eigen::VectorXd const control_timepoints;    // Order dependency before
    Eigen::VectorXd const constraint_timepoints; // Order dependency before
    Eigen::VectorXd const initial_state;
    Aux::InterpolatingVector
        objective_gradient; // Order dependency after problem and timepoints
    ConstraintJacobian
        constraint_jacobian; // Order dependency after problem and timepoints

    bool
    compute_states_from_controls(Aux::InterpolatingVector_Base const &controls);

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

  public:
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
