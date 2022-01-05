#include "ImplicitOptimizer.hpp"
#include "ControlStateCache.hpp"
#include "Exception.hpp"
#include "Initialvalues.hpp"
#include "Matrixhandler.hpp"
#include "OptimizableObject.hpp"
#include "Optimization_helpers.hpp"
#include "Optimizer.hpp"
#include <memory>
#include <string>

namespace Optimization {
  template <typename T>
  static inline T back(Eigen::Ref<Eigen::VectorX<T> const> const &vector) {
    return vector[vector.size() - 1];
  }

  template <typename T> static inline T back(Eigen::VectorX<T> const &vector) {
    return vector[vector.size() - 1];
  }

  template <typename T>
  static inline T &back(Eigen::Ref<Eigen::VectorX<T>> &vector) {
    return vector[vector.size() - 1];
  }

  template <typename T> static inline T &back(Eigen::VectorX<T> &vector) {
    return vector[vector.size() - 1];
  }

  template <typename T>
  static inline Eigen::Index
  back_index(Eigen::Ref<Eigen::VectorX<T> const> const &vector) {
    return vector.size() - 1;
  }
  template <typename T>
  static inline Eigen::Index back_index(Eigen::VectorX<T> const &vector) {
    return vector.size() - 1;
  }

  template <typename T>
  static inline T front(Eigen::Ref<Eigen::VectorX<T> const> const &vector) {
    return vector[0];
  }

  template <typename T> static inline T front(Eigen::VectorX<T> const &vector) {
    return vector[0];
  }

  template <typename T>
  static inline T &front(Eigen::Ref<Eigen::VectorX<T>> &vector) {
    return vector[0];
  }

  template <typename T> static inline T &front(Eigen::VectorX<T> &vector) {
    return vector[0];
  }

  static Eigen::VectorXd make_objective_function_coefficients(
      Eigen::Ref<Eigen::VectorXd const> timepoints) {
    assert(timepoints.size() > 0);
    Eigen::VectorXd coefficients(timepoints.size());
    if (coefficients.size() == 1) {
      front(coefficients) = 1;
      return coefficients;
    }
    // first timepoint:
    front(coefficients) = 0.5 * (timepoints[1] - timepoints[0]);
    for (Eigen::Index index = 1; index != back_index(timepoints); ++index) {
      coefficients[index]
          = 0.5 * (timepoints[index + 1] - timepoints[index - 1]);
    }
    back(coefficients) = 0.5
                         * (timepoints[back_index(timepoints)]
                            - timepoints[back_index(timepoints) - 1]);
    return coefficients;
  }

  ImplicitOptimizer::ImplicitOptimizer(
      std::unique_ptr<Model::OptimizableObject> _problem,
      std::unique_ptr<StateCache> _cache,
      Eigen::Ref<Eigen::VectorXd const> const &_state_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &_control_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &_constraint_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &_initial_state,
      Aux::InterpolatingVector_Base const &_initial_controls,
      Aux::InterpolatingVector_Base const &_lower_bounds,
      Aux::InterpolatingVector_Base const &_upper_bounds,
      Aux::InterpolatingVector_Base const &_constraint_lower_bounds,
      Aux::InterpolatingVector_Base const &_constraint_upper_bounds) :
      problem(std::move(_problem)),
      init(std::make_unique<Initialvalues>(
          _initial_controls, _lower_bounds, _upper_bounds,
          _constraint_lower_bounds, _constraint_upper_bounds)),
      cache(std::move(_cache)),
      state_timepoints(_state_timepoints),
      control_timepoints(_control_timepoints),
      constraint_timepoints(_constraint_timepoints),
      initial_state(_initial_state),
      index_lambda_pairs(
          compute_index_lambda_vector(control_timepoints, state_timepoints)),
      integral_weights(make_objective_function_coefficients(state_timepoints)),
      objective_gradient(control_timepoints, controls_per_step()),
      constraint_jacobian(
          constraints_per_step(), controls_per_step(), constraint_timepoints,
          control_timepoints),
      constraintjacobian_accessor(
          nullptr, constraint_jacobian.nonZeros(), constraints_per_step(),
          controls_per_step(), constraint_timepoints, control_timepoints) {
    // control sanity checks:
    if (problem->get_number_of_controls_per_timepoint()
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
    if (problem->get_number_of_constraints_per_timepoint()
        != init->constraint_lower_bounds.get_inner_length()) {
      gthrow(
          {"Number of constraints and number of constraint bounds do not "
           "match!"});
    }

    // state sanity checks:
    if (problem->get_number_of_states() != initial_state.size()) {
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
    if (back(control_timepoints) < back(state_timepoints)) {
      gthrow(
          {"latest state timepoint is not inside the control timepoint span."});
    }

    // check that first control timepoint is before or at first state
    // timepoint.
    if (control_timepoints(0) > state_timepoints(0)) {
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
      } else {
        // FAIL: a constraint time lies in between two state times!
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

  ImplicitOptimizer::~ImplicitOptimizer() = default;

  ////////////////////////////////////////////////////////////
  // Overrides:
  ////////////////////////////////////////////////////////////

  bool ImplicitOptimizer::supply_constraint_jacobian_indices(
      Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Rowindices,
      Eigen::Ref<Eigen::VectorX<Ipopt::Index>> Colindices) const {
    constraint_jacobian.supply_indices(Rowindices, Colindices);
    return true;
  }

  Eigen::Index ImplicitOptimizer::get_total_no_controls() const {
    return controls_per_step() * control_steps();
  }
  Eigen::Index ImplicitOptimizer::get_total_no_constraints() const {
    return constraints_per_step() * constraint_steps();
  }
  Eigen::Index ImplicitOptimizer::get_no_nnz_in_jacobian() const {
    return constraint_jacobian.nonZeros();
  }

  void ImplicitOptimizer::new_x() {
    states_up_to_date = false;
    derivatives_up_to_date = false;
  }

  std::tuple<bool, bool, bool> ImplicitOptimizer::get_boolians() const {
    return {
        derivative_matrices_initialized, states_up_to_date,
        derivatives_up_to_date};
  }

  bool ImplicitOptimizer::evaluate_objective(
      Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
      double &objective) {
    assert(ipoptcontrols.size() == get_total_no_controls());

    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), ipoptcontrols.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));

    if (not states_up_to_date) {
      auto could_compute_states = cache->refresh_cache(
          *problem, controls, state_timepoints, initial_state);
      if (not could_compute_states) {
        return false;
      }
    }
    auto &states = cache->get_cached_states();

    objective = 0;
    // timeindex starts at 1, because at 0 there are initial conditions
    // which can not be altered!
    for (Eigen::Index timeindex = 1; timeindex != states.size(); ++timeindex) {
      objective += integral_weights[timeindex]
                   * problem->evaluate_cost(
                       state_timepoints[timeindex],
                       states(state_timepoints[timeindex]),
                       controls(state_timepoints[timeindex]));
    }
    return true;
  }

  bool ImplicitOptimizer::evaluate_constraints(
      Eigen::Ref<Eigen::VectorXd const> const &ipoptcontrols,
      Eigen::Ref<Eigen::VectorXd> ipoptconstraints) {
    assert(ipoptconstraints.size() == get_total_no_constraints());
    assert(ipoptcontrols.size() == get_total_no_controls());

    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), ipoptcontrols.data(),
        static_cast<Eigen::Index>(get_total_no_controls()));

    // get states:
    if (not states_up_to_date) {
      auto could_compute_states = cache->refresh_cache(
          *problem, controls, state_timepoints, initial_state);
      if (not could_compute_states) {
        return false;
      }
    }
    auto &states = cache->get_cached_states();

    Aux::MappedInterpolatingVector constraints(
        constraint_timepoints, constraints_per_step(), ipoptconstraints.data(),
        static_cast<Eigen::Index>(get_total_no_constraints()));

    // fill the constraints vector for every (constraints-)timepoint
    for (Eigen::Index constraint_timeindex = 0;
         constraint_timeindex != constraints.size(); ++constraint_timeindex) {
      double time
          = constraints.interpolation_point_at_index(constraint_timeindex);
      problem->evaluate_constraint(
          constraints.mut_timestep(constraint_timeindex), time, states(time),
          controls(time));
    }
    return true;
  }

  bool ImplicitOptimizer::evaluate_objective_gradient(
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
      auto could_compute_states = cache->refresh_cache(
          *problem, controls, state_timepoints, initial_state);
      if (not could_compute_states) {
        return false;
      }
    }
    auto &states = cache->get_cached_states();
    auto could_compute_derivatives = compute_derivatives(controls, states);
    if (not could_compute_derivatives) {
      return false;
    }
    gradient = objective_gradient;
    return true;
  }
  bool ImplicitOptimizer::evaluate_constraint_jacobian(
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
      auto could_compute_states = cache->refresh_cache(
          *problem, controls, state_timepoints, initial_state);
      if (not could_compute_states) {
        return false;
      }
    }
    auto &states = cache->get_cached_states();
    auto could_compute_derivatives = compute_derivatives(controls, states);
    if (not could_compute_derivatives) {
      return false;
    }
    constraintjacobian_accessor = constraint_jacobian;
    return true;
  }

  // initial values:

  Eigen::Ref<Eigen::VectorXd const> ImplicitOptimizer::get_initial_state() {
    return initial_state;
  }

  Eigen::VectorXd ImplicitOptimizer::get_initial_controls() {
    // Check that init has not been deleted:
    assert(init);
    auto initial_controls
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(), init->initial_controls);
    return initial_controls.get_allvalues();
  }
  Eigen::VectorXd ImplicitOptimizer::get_lower_bounds() {
    // Check that init has not been deleted:
    assert(init);
    auto lower_bounds
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(), init->lower_bounds);
    return lower_bounds.get_allvalues();
  }
  Eigen::VectorXd ImplicitOptimizer::get_upper_bounds() {
    // Check that init has not been deleted:
    assert(init);
    auto upper_bounds
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(), init->upper_bounds);
    return upper_bounds.get_allvalues();
  }
  Eigen::VectorXd ImplicitOptimizer::get_constraint_lower_bounds() {
    // Check that init has not been deleted:
    assert(init);
    auto constraint_lower_bounds
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(),
            init->constraint_lower_bounds);
    return constraint_lower_bounds.get_allvalues();
  }
  Eigen::VectorXd ImplicitOptimizer::get_constraint_upper_bounds() {
    // Check that init has not been deleted:
    assert(init);
    auto constraint_upper_bounds
        = Aux::InterpolatingVector::construct_and_interpolate_from(
            control_timepoints, controls_per_step(),
            init->constraint_upper_bounds);
    return constraint_upper_bounds.get_allvalues();
  }
  ////////////////////////////////////////////////////////////
  // End of Overrides
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // Internal methods:
  ////////////////////////////////////////////////////////////

  bool ImplicitOptimizer::compute_derivatives(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states) {

    if (not derivative_matrices_initialized) {
      initialize_derivative_matrices(controls, states);
    }

    // Here we go backwards through the timesteps:
    {
      objective_gradient.setZero();
      constraint_jacobian.setZero();

      // TODO: should be preallocated:
      Eigen::VectorXd xi_f(states_per_step());
      Eigen::VectorXd rhs_f = Eigen::VectorXd::Zero(states_per_step());
      Eigen::RowVectorXd df_dui(controls_per_step());

      Eigen::MatrixXd full_Xi_row = Eigen::MatrixXd::Zero(
          states_per_step(), get_total_no_constraints());
      Eigen::MatrixXd full_rhs_g = Eigen::MatrixXd::Zero(
          states_per_step(), get_total_no_constraints());
      RowMat full_dg_dui = Eigen::MatrixXd::Zero(
          get_total_no_constraints(), controls_per_step());

      Eigen::Index state_index = back_index(state_timepoints);
      // We set this to
      Eigen::Index constraint_index = constraint_steps();

      bool constraints_are_active_at_current_time = false;
      while (state_index > 0) {
        update_equation_derivative_matrices(state_index, controls, states);

        {
          // cost derivative:
          update_cost_derivative_matrices(state_index, controls, states);
          rhs_f -= integral_weights[state_index] * df_dnew_transposed;
          xi_f = solver.solve(rhs_f);
          rhs_f.noalias() = -dE_dlast_transposed * xi_f;
          df_dui.noalias() = xi_f.transpose() * dE_dcontrol;
          df_dui += integral_weights[state_index] * df_dcontrol;

          // Compute the actual derivative with respect to the control:
          auto lambda = index_lambda_pairs[state_index].second;
          assert(0 <= lambda);
          assert(lambda <= 1.0);
          auto upper_index = index_lambda_pairs[state_index].first;
          if (lambda == 1.0) {
            objective_gradient.mut_timestep(upper_index) += df_dui;
          } else {
            objective_gradient.mut_timestep(upper_index) += lambda * df_dui;
            objective_gradient.mut_timestep(upper_index - 1)
                += (1 - lambda) * df_dui;
          }
        }
        {
          // constraints:
          // Find out, whether a new constraint appears at this index:
          bool current_step_is_new_constraint_time = false;
          if (constraint_timepoints[constraint_index - 1]
              == state_timepoints[state_index]) {
            constraints_are_active_at_current_time = true;
            --constraint_index;
            current_step_is_new_constraint_time = true;
          }

          if (constraints_are_active_at_current_time) {
            if (current_step_is_new_constraint_time) {
              update_constraint_derivative_matrices(
                  state_index, controls, states);
              assert(middle_col_block(full_rhs_g, constraint_index).isZero());
              middle_col_block(full_rhs_g, constraint_index)
                  -= dg_dnew_transposed;
            }

            auto current_Xi = right_cols(full_Xi_row, constraint_index);
            auto current_rhs = right_cols(full_rhs_g, constraint_index);
            // std::cout << "current_rhs\n" << current_rhs << std::endl;
            current_Xi = solver.solve(current_rhs);
            // std::cout << "current_Xi\n" << current_Xi << std::endl;
            current_rhs.noalias() = -dE_dlast_transposed * current_Xi;
            auto current_dg_dui = lower_rows(full_dg_dui, constraint_index);
            current_dg_dui.noalias() = current_Xi.transpose() * dE_dcontrol;

            if (current_step_is_new_constraint_time) {
              middle_row_block(full_dg_dui, constraint_index) += dg_dcontrol;
            }

            // Compute the actual derivative with respect to the control:
            auto lambda = index_lambda_pairs[state_index].second;
            assert(0 <= lambda);
            assert(lambda <= 1.0);
            auto upper_index = index_lambda_pairs[state_index].first;
            if (lambda == 1.0) {
              lower_rows(
                  constraint_jacobian.get_column_block(upper_index),
                  constraint_index)
                  += current_dg_dui;
            } else {
              lower_rows(
                  constraint_jacobian.get_column_block(upper_index),
                  constraint_index)
                  += lambda * current_dg_dui;
              lower_rows(
                  constraint_jacobian.get_column_block(upper_index - 1),
                  constraint_index)
                  += (1 - lambda) * current_dg_dui;
            }
          }
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
  void ImplicitOptimizer::initialize_derivative_matrices(
      Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states) {
    double last_time = state_timepoints[back_index(state_timepoints) - 1];
    double new_time = back(state_timepoints);

    dE_dnew_transposed.resize(states_per_step(), states_per_step());
    Aux::Triplethandler<Aux::Transposed> new_handler(dE_dnew_transposed);
    problem->d_evalutate_d_new_state(
        new_handler, last_time, new_time, states(last_time), states(new_time),
        controls(new_time));
    new_handler.set_matrix();
    solver.analyzePattern(dE_dnew_transposed);

    dE_dlast_transposed.resize(states_per_step(), states_per_step());
    Aux::Triplethandler<Aux::Transposed> last_handler(dE_dlast_transposed);
    problem->d_evalutate_d_last_state(
        last_handler, last_time, new_time, states(last_time), states(new_time),
        controls(new_time));
    last_handler.set_matrix();

    dE_dcontrol.resize(states_per_step(), controls_per_step());
    Aux::Triplethandler control_handler(dE_dcontrol);
    problem->d_evalutate_d_control(
        control_handler, last_time, new_time, states(last_time),
        states(new_time), controls(new_time));
    control_handler.set_matrix();

    dg_dnew_transposed.resize(states_per_step(), constraints_per_step());
    Aux::Triplethandler<Aux::Transposed> gnew_handler(dg_dnew_transposed);
    problem->d_evaluate_constraint_d_state(
        gnew_handler, new_time, states(new_time), controls(new_time));
    gnew_handler.set_matrix();

    dg_dcontrol.resize(constraints_per_step(), controls_per_step());
    Aux::Triplethandler gcontrol_handler(dg_dcontrol);
    problem->d_evaluate_constraint_d_control(
        gcontrol_handler, new_time, states(new_time), controls(new_time));
    gcontrol_handler.set_matrix();

    df_dnew_transposed.resize(states_per_step(), 1);
    Aux::Triplethandler<Aux::Transposed> fnew_handler(df_dnew_transposed);
    problem->d_evaluate_cost_d_state(
        fnew_handler, new_time, states(new_time), controls(new_time));
    fnew_handler.set_matrix();

    df_dcontrol.resize(1, controls_per_step());
    Aux::Triplethandler fcontrol_handler(df_dcontrol);
    problem->d_evaluate_cost_d_control(
        fcontrol_handler, new_time, states(new_time), controls(new_time));
    fcontrol_handler.set_matrix();

    // A_jp1_Lambda_j
    //     = Eigen::MatrixXd::Zero(states_per_step(),
    //     get_total_no_constraints());
    // Lambda_j
    //     = Eigen::MatrixXd::Zero(states_per_step(),
    //     get_total_no_constraints());
    // dg_duj = RowMat::Zero(get_total_no_constraints(), controls_per_step());

    derivative_matrices_initialized = true;
  }

  /** \brief fills the matrices #dE_dnew_transposed #dE_dlast_transposed and
   * #dE_dcontrol with their values at state_index and fills #solver with the
   * factorization of #dE_dnew_transposed.
   */
  void ImplicitOptimizer::update_equation_derivative_matrices(
      Eigen::Index state_index, Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states) {
    assert(derivative_matrices_initialized);
    assert(state_index > 0);
    assert(state_index < states.size());

    double last_time = state_timepoints[state_index - 1];
    double new_time = state_timepoints[state_index];

    Aux::Coeffrefhandler<Aux::Transposed> new_handler(dE_dnew_transposed);
    problem->d_evalutate_d_new_state(
        new_handler, last_time, new_time, states(last_time), states(new_time),
        controls(new_time));
    solver.factorize(dE_dnew_transposed);
    solver.analyzePattern(dE_dnew_transposed);
    Aux::Coeffrefhandler<Aux::Transposed> last_handler(dE_dlast_transposed);
    problem->d_evalutate_d_last_state(
        last_handler, last_time, new_time, states(last_time), states(new_time),
        controls(new_time));

    Aux::Coeffrefhandler control_handler(dE_dcontrol);
    problem->d_evalutate_d_control(
        control_handler, last_time, new_time, states(last_time),
        states(new_time), controls(new_time));
  }

  void ImplicitOptimizer::update_constraint_derivative_matrices(
      Eigen::Index state_index, Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states) {
    assert(derivative_matrices_initialized);
    assert(state_index > 0);
    assert(state_index < states.size());

    double time = state_timepoints[state_index];

    Aux::Coeffrefhandler<Aux::Transposed> gnew_handler(dg_dnew_transposed);
    problem->d_evaluate_constraint_d_state(
        gnew_handler, time, states(time), controls(time));
    Aux::Coeffrefhandler gcontrol_handler(dg_dcontrol);
    problem->d_evaluate_constraint_d_control(
        gcontrol_handler, time, states(time), controls(time));
  }

  void ImplicitOptimizer::update_cost_derivative_matrices(
      Eigen::Index state_index, Aux::InterpolatingVector_Base const &controls,
      Aux::InterpolatingVector_Base const &states) {
    assert(derivative_matrices_initialized);
    assert(state_index > 0);
    assert(state_index < states.size());

    double time = state_timepoints[state_index];

    Aux::Coeffrefhandler<Aux::Transposed> fnew_handler(df_dnew_transposed);
    problem->d_evaluate_cost_d_state(
        fnew_handler, time, states(time), controls(time));
    Aux::Coeffrefhandler fcontrol_handler(df_dcontrol);
    problem->d_evaluate_cost_d_control(
        fcontrol_handler, time, states(time), controls(time));
  }

  ////////////////////////////////////////////////////////////
  // End of Internal methods
  ////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////
  // getters
  ////////////////////////////////////////////////////////////

  Aux::InterpolatingVector_Base const &
  ImplicitOptimizer::get_current_full_state() const {
    return cache->get_cached_states();
  }
  Aux::InterpolatingVector_Base const &
  ImplicitOptimizer::get_objective_gradient() const {
    return objective_gradient;
  }
  ConstraintJacobian_Base const &
  ImplicitOptimizer::get_constraint_jacobian() const {
    return constraint_jacobian;
  }

  ////////////////////////////////////////////////////////////
  // simple convenience methods:
  ////////////////////////////////////////////////////////////

  Eigen::Ref<Eigen::VectorXd const>
  ImplicitOptimizer::get_integral_weights() const {
    return integral_weights;
  }

  Eigen::Index ImplicitOptimizer::states_per_step() const {
    return problem->get_number_of_states();
  }
  Eigen::Index ImplicitOptimizer::controls_per_step() const {
    return problem->get_number_of_controls_per_timepoint();
  }
  Eigen::Index ImplicitOptimizer::constraints_per_step() const {
    return problem->get_number_of_constraints_per_timepoint();
  }
  Eigen::Index ImplicitOptimizer::state_steps() const {
    return state_timepoints.size();
  }
  Eigen::Index ImplicitOptimizer::control_steps() const {
    return control_timepoints.size();
  }
  Eigen::Index ImplicitOptimizer::constraint_steps() const {
    return constraint_timepoints.size();
  }
  ////////////////////////////////////////////////////////////
  // End of simple convenience methods:
  ////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////
  // Matrix block methods:
  ////////////////////////////////////////////////////////////
  Eigen::Ref<Eigen::MatrixXd> ImplicitOptimizer::right_cols(
      Eigen::Ref<Eigen::MatrixXd> Fullmat, Eigen::Index outer_col_index) const {
    assert(Fullmat.rows() == states_per_step());
    assert(Fullmat.cols() == get_total_no_constraints());
    assert(outer_col_index >= 0);
    assert(outer_col_index < constraint_steps());

    return Fullmat.rightCols(
        get_total_no_constraints() - outer_col_index * constraints_per_step());
  }

  Eigen::Ref<Eigen::MatrixXd> ImplicitOptimizer::middle_col_block(
      Eigen::Ref<Eigen::MatrixXd> Fullmat, Eigen::Index outer_col_index) const {
    assert(Fullmat.rows() == states_per_step());
    assert(Fullmat.cols() == get_total_no_constraints());
    assert(outer_col_index >= 0);
    assert(outer_col_index < constraint_steps());

    return Fullmat.middleCols(
        outer_col_index * constraints_per_step(), constraints_per_step());
  }

  Eigen::Ref<RowMat> ImplicitOptimizer::lower_rows(
      Eigen::Ref<RowMat> Fullmat, Eigen::Index outer_row_index) const {
    // assert(Fullmat.rows() == get_total_no_constraints());
    assert(Fullmat.cols() == controls_per_step());
    assert(outer_row_index >= 0);
    assert(outer_row_index < constraint_steps());

    return Fullmat.bottomRows(
        get_total_no_constraints() - outer_row_index * constraints_per_step());
  }

  Eigen::Ref<RowMat> ImplicitOptimizer::middle_row_block(
      Eigen::Ref<RowMat> Fullmat, Eigen::Index outer_row_index) const {
    assert(Fullmat.rows() == get_total_no_constraints());
    assert(Fullmat.cols() == controls_per_step());
    assert(outer_row_index >= 0);
    assert(outer_row_index < constraint_steps());
    return Fullmat.middleRows(
        outer_row_index * constraints_per_step(), constraints_per_step());
  }

} // namespace Optimization
