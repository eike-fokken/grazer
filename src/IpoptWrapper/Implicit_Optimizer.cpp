#include "Implicit_Optimizer.hpp"
#include "Exception.hpp"
#include "Initialvalues.hpp"
#include "Matrixhandler.hpp"
#include "OptimizableObject.hpp"
#include "Optimization_helpers.hpp"
#include <memory>

namespace Optimization {

  Implicit_Optimizer::Implicit_Optimizer(
      Model::OptimizableObject &_problem, Model::Timeevolver &_evolver,
      Eigen::Ref<Eigen::VectorXd const> const &_state_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &_control_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &_constraint_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &_initial_state,
      Aux::InterpolatingVector_Base const &_initial_controls,
      Aux::InterpolatingVector_Base const &_lower_bounds,
      Aux::InterpolatingVector_Base const &_upper_bounds,
      Aux::InterpolatingVector_Base const &_constraint_lower_bounds,
      Aux::InterpolatingVector_Base const &_constraint_upper_bounds) :
      problem(_problem),
      init(std::make_unique<Initialvalues>(
          _initial_controls, _lower_bounds, _upper_bounds,
          _constraint_lower_bounds, _constraint_upper_bounds)),
      cache(_evolver, _problem),
      state_timepoints(_state_timepoints),
      control_timepoints(_control_timepoints),
      constraint_timepoints(_constraint_timepoints),
      initial_state(_initial_state),
      objective_gradient(control_timepoints, controls_per_step()),
      constraint_jacobian(
          constraints_per_step(), controls_per_step(), constraint_timepoints,
          control_timepoints),
      constraintjacobian_accessor(
          nullptr, constraint_jacobian.nonZeros(), constraints_per_step(),
          controls_per_step(), constraint_timepoints, control_timepoints) {
    // control sanity checks:
    if (problem.get_number_of_controls_per_timepoint()
        != init->initial_controls.get_inner_length()) {
      gthrow({"Wrong number of initial values of the controls!"});
    }
    if (not have_same_structure(init->initial_controls, init->lower_bounds)) {
      gthrow(
          {"Number of controls and number of control bounds do not match !"});
    }
    if (not have_same_structure(init->lower_bounds, init->upper_bounds)) {
      gthrow({"The lower and upper bounds do not match!"});
    }
    // constraints sanity checks:
    if (not have_same_structure(
            init->constraint_lower_bounds, init->constraint_upper_bounds)) {
      gthrow({"The lower and upper bounds for the constraints do not match!"});
    }
    if (problem.get_number_of_constraints_per_timepoint()
        != init->constraint_lower_bounds.get_inner_length()) {
      gthrow(
          {"Number of constraints and number of constraint bounds do not "
           "match!"});
    }

    // state sanity checks:
    if (problem.get_number_of_states() != initial_state.size()) {
      gthrow({"Wrong number of initial values of the states!"});
    }
    // check all timepoints are sorted:
    if (not(std::is_sorted(
            state_timepoints.cbegin(), state_timepoints.cend()))) {
      gthrow({"The state timepoints are not sorted!"});
    }
    if (not(std::is_sorted(
            control_timepoints.cbegin(), control_timepoints.cend()))) {
      gthrow({"The control timepoints are not sorted!"});
    }
    if (not(std::is_sorted(
            constraint_timepoints.cbegin(), constraint_timepoints.cend()))) {
      gthrow({"The constraints timepoints are not sorted!"});
    }

    // check that latest control timepoint is after or at latest state
    // timepoint:
    if (control_timepoints(control_timepoints.size() - 1)
        < state_timepoints(state_timepoints.size() - 1)) {
      gthrow(
          {"latest state timepoint is not inside the control timepoint span."});
    }

    // check that first control timepoint is before or at latest state
    // timepoint. Note that at the state timepoint at 0, no controls are ever
    // evaluated, as here only the state initial values are given.
    if (control_timepoints(0) > state_timepoints(1)) {
      gthrow(
          {"first state timepoint is not inside the control timepoint span."});
    }

    // check whether constraint timepoints are a subset of state timepoints
    // and that constraint times start at or after the time at
    // state_timepoints[1]
    auto constraint_iterator = constraint_timepoints.begin();

    // If we are past all constraints and no error was detected, we are
    // clear:
    for (Eigen::Index i = 1; i != state_timepoints.size(); ++i) {
      if (constraint_iterator == constraint_timepoints.end()) {
        break;
      }
      auto timepoint = state_timepoints[i];
      // next constraint time is further ahead:
      if (timepoint < *constraint_iterator) {
        continue;
        // found next constraint time is equal to a state time.
      } else if (timepoint == *constraint_iterator) {
        ++constraint_iterator;
        // FAIL: a constraint time lies in between two state times!
      } else {
        gthrow(
            {"Constraint time ", std::to_string(*constraint_iterator),
             " is not a state time. This is a bug."});
      }
    }
    if (constraint_iterator != constraint_timepoints.end()) {
      // FAIL: a constraint time lies after all state times:
      gthrow(
          {"Constraint time ", std::to_string(*constraint_iterator),
           " is not a state time. This is a bug."});
    }
  }

  Implicit_Optimizer::~Implicit_Optimizer() = default;

  ////////////////////////////////////////////////////////////
  // Overrides:
  ////////////////////////////////////////////////////////////

  bool Implicit_Optimizer::supply_constraint_jacobian_indices(
      Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Rowindices,
      Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Colindices) const {
    constraint_jacobian.supply_indices(Rowindices, Colindices);
    return true;
  }

  Eigen::Index Implicit_Optimizer::get_total_no_controls() const {
    return controls_per_step() * control_steps();
  }
  Eigen::Index Implicit_Optimizer::get_total_no_constraints() const {
    return constraints_per_step() * constraint_steps();
  }
  Eigen::Index Implicit_Optimizer::get_no_nnz_in_jacobian() const {
    return constraint_jacobian.nonZeros();
  }

  void Implicit_Optimizer::new_x() {
    states_up_to_date = false;
    derivatives_up_to_date = false;
  }

  bool Implicit_Optimizer::evaluate_objective(
      Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
      double &objective) {
    assert(ipoptcontrols.size() == get_total_no_controls());

    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), ipoptcontrols.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));

    if (not states_up_to_date) {
      auto could_compute_states
          = cache.refresh_cache(controls, state_timepoints, initial_state);
      if (not could_compute_states) {
        return false;
      }
    }
    auto &states = cache.get_cached_states();

    objective = 0;
    // timeindex starts at 1, because at 0 there are initial conditions
    // which can not be altered!
    for (Eigen::Index timeindex = 1; timeindex != states.size(); ++timeindex) {
      objective += problem.evaluate_cost(
          state_timepoints[timeindex], states(state_timepoints[timeindex]),
          controls(state_timepoints[timeindex]));
    }
    return true;
  }

  bool Implicit_Optimizer::evaluate_constraints(
      Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
      Eigen::Ref<Eigen::VectorXd> ipoptconstraints) {
    assert(ipoptconstraints.size() == get_total_no_constraints());
    assert(ipoptcontrols.size() == get_total_no_controls());

    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), ipoptcontrols.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));

    // get states:
    if (not states_up_to_date) {
      auto could_compute_states
          = cache.refresh_cache(controls, state_timepoints, initial_state);
      if (not could_compute_states) {
        return false;
      }
    }
    auto &states = cache.get_cached_states();

    Aux::MappedInterpolatingVector constraints(
        constraint_timepoints, constraints_per_step(), ipoptconstraints.data(),
        static_cast<Eigen::Index>(get_total_no_constraints()));

    // fill the constraints vector for every (constraints-)timepoint
    for (Eigen::Index constraint_timeindex = 0;
         constraint_timeindex != constraints.size(); ++constraint_timeindex) {
      double time
          = constraints.interpolation_point_at_index(constraint_timeindex);
      problem.evaluate_constraint(
          constraints.mut_timestep(constraint_timeindex), time, states(time),
          controls(time));
    }
    return true;
  }

  bool Implicit_Optimizer::evaluate_objective_gradient(
      Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
      Eigen::Ref<Eigen::VectorXd> ipoptgradient) {
    assert(ipoptgradient.size() == get_total_no_controls());
    assert(ipoptcontrols.size() == get_total_no_controls());

    Aux::MappedInterpolatingVector gradient(
        control_timepoints, controls_per_step(), ipoptgradient.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));
    if (derivatives_up_to_date) {
      // if we already computed this, just return the value:
      gradient = objective_gradient;
      return true;
    }

    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), ipoptcontrols.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));
    // get states:
    if (not states_up_to_date) {
      auto could_compute_states
          = cache.refresh_cache(controls, state_timepoints, initial_state);
      if (not could_compute_states) {
        return false;
      }
    }
    auto &states = cache.get_cached_states();
    auto could_compute_derivatives = compute_derivatives(controls, states);
    if (not could_compute_derivatives) {
      return false;
    }
    gradient = objective_gradient;
    return true;
  }
  bool Implicit_Optimizer::evaluate_constraint_jacobian(
      Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
      Eigen::Ref<Eigen::VectorXd> values) {
    assert(values.size() == get_no_nnz_in_jacobian());
    assert(ipoptcontrols.size() == get_total_no_controls());
    constraintjacobian_accessor.replace_storage(values.data(), values.size());

    if (derivatives_up_to_date) {
      // if we already computed this, just return the value:
      constraintjacobian_accessor = constraint_jacobian;
      return true;
    }

    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), ipoptcontrols.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));
    // get states:
    if (not states_up_to_date) {
      auto could_compute_states
          = cache.refresh_cache(controls, state_timepoints, initial_state);
      if (not could_compute_states) {
        return false;
      }
    }
    auto &states = cache.get_cached_states();
    auto could_compute_derivatives = compute_derivatives(controls, states);
    if (not could_compute_derivatives) {
      return false;
    }
    constraintjacobian_accessor = constraint_jacobian;
    return true;
  }

  // initial values:
  Eigen::VectorXd Implicit_Optimizer::get_initial_controls() {
    // Check that init has not been deleted:
    assert(init);
    auto initial_controls
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(), init->initial_controls);
    if (init->obsolete()) {
      init.reset();
    }
    return initial_controls.get_allvalues();
  }
  Eigen::VectorXd Implicit_Optimizer::get_lower_bounds() {
    // Check that init has not been deleted:
    assert(init);
    auto lower_bounds
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(), init->lower_bounds);
    if (init->obsolete()) {
      init.reset();
    }
    return lower_bounds.get_allvalues();
  }
  Eigen::VectorXd Implicit_Optimizer::get_upper_bounds() {
    // Check that init has not been deleted:
    assert(init);
    auto upper_bounds
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(), init->upper_bounds);
    if (init->obsolete()) {
      init.reset();
    }
    return upper_bounds.get_allvalues();
  }
  Eigen::VectorXd Implicit_Optimizer::get_constraint_lower_bounds() {
    // Check that init has not been deleted:
    assert(init);
    auto constraint_lower_bounds
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(),
            init->constraint_lower_bounds);
    if (init->obsolete()) {
      init.reset();
    }
    return constraint_lower_bounds.get_allvalues();
  }
  Eigen::VectorXd Implicit_Optimizer::get_constraint_upper_bounds() {
    // Check that init has not been deleted:
    assert(init);
    auto constraint_upper_bounds
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(),
            init->constraint_upper_bounds);
    if (init->obsolete()) {
      init.reset();
    }
    return constraint_upper_bounds.get_allvalues();
  }
  ////////////////////////////////////////////////////////////
  // End of Overrides
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // Internal methods:
  ////////////////////////////////////////////////////////////

  bool Implicit_Optimizer::compute_derivatives(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states) {
    if (not derivative_matrices_initialized) {
      initialize_derivative_matrices(states, controls);
    }
    auto index_lambda_pairs
        = compute_index_lambda_vector(control_timepoints, state_timepoints);
    // Here we go backwards through the timesteps:

    {
      objective_gradient.setZero();
      constraint_jacobian.setZero();
      // TODO: should be preallocated:
      Eigen::VectorXd xi(states_per_step());
      Eigen::VectorXd ATxi = Eigen::VectorXd::Zero(states_per_step());
      Eigen::VectorXd objective_rhs(states_per_step());
      Eigen::VectorXd df_dui(controls_per_step());
      Eigen::Index state_index = state_timepoints.size() - 1;

      Eigen::MatrixXd constraint_rhs(states_per_step(), constraints_per_step());

      {
        update_equation_derivative_matrices(state_index, states, controls);
        update_cost_derivative_matrices(state_index, states, controls);

        // solve B^T xi = df/dx^T
        objective_rhs = -ATxi;
        objective_rhs -= df_dnew_transposed;
        xi = solver.solve(objective_rhs);
        ATxi.noalias() = dE_dlast_transposed * xi;
        df_dui.noalias() = xi.transpose() * dE_dcontrol;
        df_dui += df_dcontrol;

        // Compute the actual derivative with respect to the control:
        auto lambda = index_lambda_pairs[state_index].second;
        auto upper_index = index_lambda_pairs[state_index].first;
        if (lambda == 1.0) {
          objective_gradient.mut_timestep(upper_index) += df_dui;
        } else {
          objective_gradient.mut_timestep(upper_index) += lambda * df_dui;
          objective_gradient.mut_timestep(upper_index - 1)
              += (1 - lambda) * df_dui;
        }
        --state_index;
      }
      Eigen::Index constraint_index = constraint_steps() - 1;
      while (state_index > 0) {
        update_equation_derivative_matrices(state_index, states, controls);
        update_cost_derivative_matrices(state_index, states, controls);
        // solve B^T xi = df/dx^T
        objective_rhs = -ATxi;
        objective_rhs -= df_dnew_transposed;
        xi = solver.solve(objective_rhs);
        ATxi.noalias() = dE_dlast_transposed * xi;
        df_dui.noalias() = xi.transpose() * dE_dcontrol;
        df_dui += df_dcontrol;

        // constraints:
        if (constraint_timepoints[constraint_index]
            == state_timepoints[state_index]) {
          update_constraint_derivative_matrices(state_index, states, controls);

          // hier weiter!
          //////////////////////////////////////////////////////////
          static_assert(false, "hier ueber die Bloecke nachdenken!");
          auto A_block = right_cols(A_jp1_Lambda_j, constraint_index);
          auto Lambda_block = middle_col_block(Lambda_j, constraint_index);
          auto dg_duj_block = middle_row_block(dg_duj, constraint_index);

          constraint_rhs = -A_block;
          constraint_rhs -= dg_dnew_transposed;
          A_block.noalias() = dE_dlast_transposed * Lambda_block;
          // finally compute lambda_{nn}:
          Lambda_block.noalias() = solver.solve(constraint_rhs);

          dg_duj_block = (Lambda_block.transpose() * dE_dcontrol) + dg_dcontrol;
          //////////////////////////////////////////////////////////
        }

        // Compute the actual derivative with respect to the control:
        auto lambda = index_lambda_pairs[state_index].second;
        auto upper_index = index_lambda_pairs[state_index].first;
        if (lambda == 1.0) {
          objective_gradient.mut_timestep(upper_index) += df_dui;
          constraint_jacobian.get_column_block(upper_index) += dg_duj;
        } else {
          objective_gradient.mut_timestep(upper_index) += lambda * df_dui;
          objective_gradient.mut_timestep(upper_index - 1)
              += (1 - lambda) * df_dui;

          constraint_jacobian.get_column_block(upper_index) += lambda * dg_duj;
          constraint_jacobian.get_column_block(upper_index - 1)
              += (1 - lambda) * dg_duj;
        }
        --state_index;
      }
    }

    derivatives_up_to_date = true;
    return true;
  }

  /** \brief Allocates room for the structures of all matrices used in
   * derivative calculation and sets up the solver for #dE_dnew_transposed.
   */
  void Implicit_Optimizer::initialize_derivative_matrices(
      Aux::InterpolatingVector_Base const &states,
      Aux::InterpolatingVector_Base const &controls) {
    auto last_state_index = state_timepoints.size() - 1;
    double last_time = state_timepoints[last_state_index - 1];
    double new_time = state_timepoints[last_state_index];

    dE_dnew_transposed.resize(states_per_step(), states_per_step());
    Aux::Triplethandler<Aux::Transposed> new_handler(dE_dnew_transposed);
    problem.d_evalutate_d_new_state(
        new_handler, last_time, new_time, states(last_time), states(new_time),
        controls(new_time));
    new_handler.set_matrix();
    solver.analyzePattern(dE_dnew_transposed);

    dE_dlast_transposed.resize(states_per_step(), states_per_step());
    Aux::Triplethandler<Aux::Transposed> last_handler(dE_dlast_transposed);
    problem.d_evalutate_d_last_state(
        last_handler, last_time, new_time, states(last_time), states(new_time),
        controls(new_time));
    last_handler.set_matrix();

    dE_dcontrol.resize(states_per_step(), controls_per_step());
    Aux::Triplethandler control_handler(dE_dcontrol);
    problem.d_evalutate_d_control(
        control_handler, last_time, new_time, states(last_time),
        states(new_time), controls(new_time));
    control_handler.set_matrix();

    dg_dnew_transposed.resize(states_per_step(), constraints_per_step());
    Aux::Triplethandler<Aux::Transposed> gnew_handler(dg_dnew_transposed);
    problem.d_evaluate_constraint_d_state(
        gnew_handler, new_time, states(new_time), controls(new_time));
    gnew_handler.set_matrix();

    dg_dcontrol.resize(constraints_per_step(), controls_per_step());
    Aux::Triplethandler gcontrol_handler(dg_dcontrol);
    problem.d_evaluate_constraint_d_control(
        gcontrol_handler, new_time, states(new_time), controls(new_time));
    gcontrol_handler.set_matrix();

    df_dnew_transposed.resize(states_per_step(), 1);
    Aux::Triplethandler<Aux::Transposed> fnew_handler(df_dnew_transposed);
    problem.d_evaluate_cost_d_state(
        fnew_handler, new_time, states(new_time), controls(new_time));
    fnew_handler.set_matrix();

    df_dcontrol.resize(controls_per_step(), 1);
    Aux::Triplethandler fcontrol_handler(df_dcontrol);
    problem.d_evaluate_cost_d_control(
        fcontrol_handler, new_time, states(new_time), controls(new_time));
    fcontrol_handler.set_matrix();

    A_jp1_Lambda_j
        = Eigen::MatrixXd::Zero(states_per_step(), get_total_no_constraints());
    Lambda_j
        = Eigen::MatrixXd::Zero(states_per_step(), get_total_no_constraints());
    dg_duj = RowMat::Zero(get_total_no_constraints(), controls_per_step());

    derivative_matrices_initialized = true;
  }

  /** \brief fills the matrices #dE_dnew_transposed #dE_dlast_transposed and
   * #dE_dcontrol with their values at state_index and fills #solver with the
   * factorization of #dE_dnew_transposed.
   */
  void Implicit_Optimizer::update_equation_derivative_matrices(
      Eigen::Index state_index, Aux::InterpolatingVector_Base const &states,
      Aux::InterpolatingVector_Base const &controls) {
    assert(derivative_matrices_initialized);
    assert(state_index > 0);
    assert(state_index < states.size());

    double last_time = state_timepoints[state_index - 1];
    double new_time = state_timepoints[state_index];

    Aux::Coeffrefhandler<Aux::Transposed> new_handler(dE_dnew_transposed);
    problem.d_evalutate_d_new_state(
        new_handler, last_time, new_time, states(last_time), states(new_time),
        controls(new_time));
    solver.factorize(dE_dnew_transposed);
    solver.analyzePattern(dE_dnew_transposed);
    Aux::Coeffrefhandler<Aux::Transposed> last_handler(dE_dlast_transposed);
    problem.d_evalutate_d_last_state(
        last_handler, last_time, new_time, states(last_time), states(new_time),
        controls(new_time));

    Aux::Coeffrefhandler control_handler(dE_dcontrol);
    problem.d_evalutate_d_control(
        control_handler, last_time, new_time, states(last_time),
        states(new_time), controls(new_time));
  }

  void Implicit_Optimizer::update_constraint_derivative_matrices(
      Eigen::Index state_index, Aux::InterpolatingVector_Base const &states,
      Aux::InterpolatingVector_Base const &controls) {
    assert(derivative_matrices_initialized);
    assert(state_index > 0);
    assert(state_index < states.size());

    double time = state_timepoints[state_index];

    Aux::Coeffrefhandler<Aux::Transposed> gnew_handler(dg_dnew_transposed);
    problem.d_evaluate_constraint_d_state(
        gnew_handler, time, states(time), controls(time));
    Aux::Coeffrefhandler gcontrol_handler(dg_dcontrol);
    problem.d_evaluate_constraint_d_control(
        gcontrol_handler, time, states(time), controls(time));
  }

  void Implicit_Optimizer::update_cost_derivative_matrices(
      Eigen::Index state_index, Aux::InterpolatingVector_Base const &states,
      Aux::InterpolatingVector_Base const &controls) {
    assert(derivative_matrices_initialized);
    assert(state_index > 0);
    assert(state_index < states.size());

    double time = state_timepoints[state_index];

    Aux::Coeffrefhandler<Aux::Transposed> fnew_handler(df_dnew_transposed);
    problem.d_evaluate_cost_d_state(
        fnew_handler, time, states(time), controls(time));
    Aux::Coeffrefhandler fcontrol_handler(df_dcontrol);
    problem.d_evaluate_cost_d_control(
        fcontrol_handler, time, states(time), controls(time));
  }

  ////////////////////////////////////////////////////////////
  // End of Internal methods
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // simple convenience methods:
  ////////////////////////////////////////////////////////////

  Eigen::Index Implicit_Optimizer::states_per_step() const {
    return problem.get_number_of_states();
  }
  Eigen::Index Implicit_Optimizer::controls_per_step() const {
    return problem.get_number_of_controls_per_timepoint();
  }
  Eigen::Index Implicit_Optimizer::constraints_per_step() const {
    return problem.get_number_of_constraints_per_timepoint();
  }
  Eigen::Index Implicit_Optimizer::state_steps() const {
    return state_timepoints.size();
  }
  Eigen::Index Implicit_Optimizer::control_steps() const {
    return control_timepoints.size();
  }
  Eigen::Index Implicit_Optimizer::constraint_steps() const {
    return constraint_timepoints.size();
  }
  ////////////////////////////////////////////////////////////
  // End of simple convenience methods:
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // Matrix block methods:
  ////////////////////////////////////////////////////////////
  Eigen::Ref<Eigen::MatrixXd> Implicit_Optimizer::right_cols(
      Eigen::Ref<Eigen::MatrixXd> Fullmat, Eigen::Index outer_col_index) const {
    assert(Fullmat.rows() == states_per_step());
    assert(Fullmat.cols() == controls_per_step());
    assert(outer_col_index >= 0);
    assert(outer_col_index < constraint_steps());

    return Fullmat.rightCols(
        get_total_no_constraints() - outer_col_index * constraints_per_step());
  }

  Eigen::Ref<Eigen::MatrixXd> Implicit_Optimizer::middle_col_block(
      Eigen::Ref<Eigen::MatrixXd> Fullmat, Eigen::Index outer_col_index) const {
    assert(Fullmat.rows() == states_per_step());
    assert(Fullmat.cols() == get_total_no_constraints());
    assert(outer_col_index >= 0);
    assert(outer_col_index < constraint_steps());

    return Fullmat.middleCols(
        outer_col_index * constraints_per_step(), constraints_per_step());
  }

  Eigen::Ref<RowMat> Implicit_Optimizer::lower_rows(
      Eigen::Ref<RowMat> Fullmat, Eigen::Index outer_row_index) const {
    assert(Fullmat.rows() == get_total_no_constraints());
    assert(Fullmat.cols() == controls_per_step());
    assert(outer_row_index >= 0);
    assert(outer_row_index < constraint_steps());

    return Fullmat.bottomRows(
        get_total_no_constraints() - outer_row_index * constraints_per_step());
  }

  Eigen::Ref<RowMat> Implicit_Optimizer::middle_row_block(
      Eigen::Ref<RowMat> Fullmat, Eigen::Index outer_row_index) const {
    assert(Fullmat.rows() == get_total_no_constraints());
    assert(Fullmat.cols() == controls_per_step());
    assert(outer_row_index >= 0);
    assert(outer_row_index < constraint_steps());
    return Fullmat.middleRows(
        outer_row_index * constraints_per_step(), constraints_per_step());
  }

} // namespace Optimization
