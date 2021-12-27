#include "Wrapper.hpp"
#include "ConstraintJacobian.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Matrixhandler.hpp"
#include "OptimizableObject.hpp"
#include "Optimization_helpers.hpp"
#include "Timeevolver.hpp"

#include <IpAlgTypes.hpp>
#include <IpTypes.hpp>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>

namespace Optimization {

  Initialvalues::Initialvalues(
      Aux::InterpolatingVector _initial_controls,
      Aux::InterpolatingVector _lower_bounds,
      Aux::InterpolatingVector _upper_bounds,
      Aux::InterpolatingVector _constraint_lower_bounds,
      Aux::InterpolatingVector _constraint_upper_bounds) :
      initial_controls(std::move(_initial_controls)),
      lower_bounds(std::move(_lower_bounds)),
      upper_bounds(std::move(_upper_bounds)),
      constraint_lower_bounds(std::move(_constraint_lower_bounds)),
      constraint_upper_bounds(std::move(_constraint_upper_bounds)) {}

  bool Initialvalues::obsolete() const {
    return (!called_get_nlp_info) and (!called_get_bounds_info)
           and (!called_get_starting_point);
  }

  bool IpoptWrapper::evaluate_constraints(
      Ipopt::Number const *x, Ipopt::Index number_of_controls,
      Ipopt::Number *values, Ipopt::Index nele_jac) {
    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), x,
        static_cast<Eigen::Index>(number_of_controls));

    // get states:
    auto *state_pointer
        = cache.compute_states(controls, state_timepoints, initial_state);
    // if the simulation failed: tell the calling site.
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    Aux::MappedInterpolatingVector constraints(
        constraint_timepoints, constraints_per_step(), values,
        static_cast<Eigen::Index>(nele_jac));

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

  IpoptWrapper::IpoptWrapper(
      Model::Timeevolver &evolver, Model::OptimizableObject &_problem,
      Eigen::VectorXd _state_timepoints, Eigen::VectorXd _control_timepoints,
      Eigen::VectorXd _constraint_timepoints, Eigen::VectorXd _initial_state,
      Aux::InterpolatingVector _initial_controls,
      Aux::InterpolatingVector _lower_bounds,
      Aux::InterpolatingVector _upper_bounds,
      Aux::InterpolatingVector _constraint_lower_bounds,
      Aux::InterpolatingVector _constraint_upper_bounds) :
      objective_gradient(
          _initial_controls.get_interpolation_points(),
          _initial_controls.get_inner_length()),
      constraint_jacobian(
          ConstraintJacobian(_constraint_lower_bounds, _initial_controls)),
      constraint_jacobian_accessor(
          nullptr, constraint_jacobian.nonZeros(), _constraint_lower_bounds,
          _initial_controls),
      A_jp1_Lambda_j(Eigen::MatrixXd::Zero(
          _initial_state.size(),
          _constraint_lower_bounds.get_total_number_of_values())),
      Lambda_j(Eigen::MatrixXd::Zero(
          _initial_state.size(),
          _constraint_lower_bounds.get_total_number_of_values())),
      dg_duj(RowMat::Zero(
          _constraint_lower_bounds.get_total_number_of_values(),
          _initial_controls.get_inner_length())),
      problem(_problem),
      cache(evolver, _problem),
      dE_dnew_transposed(_initial_state.size(), _initial_state.size()),
      dE_dlast_transposed(_initial_state.size(), _initial_state.size()),
      dE_dcontrol(_initial_state.size(), _initial_controls.get_inner_length()),
      dg_dnew_transposed(
          _initial_state.size(), _constraint_lower_bounds.get_inner_length()),
      dg_dcontrol(
          _constraint_lower_bounds.get_inner_length(),
          _initial_controls.get_inner_length()),
      dg_dnew_dense_transposed(Eigen::MatrixXd::Zero(
          _initial_state.size(), _constraint_lower_bounds.get_inner_length())),
      state_timepoints(std::move(_state_timepoints)),
      control_timepoints(std::move(_control_timepoints)),
      constraint_timepoints(std::move(_constraint_timepoints)),
      initial_state(std::move(_initial_state)),
      init(std::make_unique<Initialvalues>(
          std::move(_initial_controls), std::move(_lower_bounds),
          std::move(_upper_bounds), std::move(_constraint_lower_bounds),
          std::move(_constraint_upper_bounds))),
      solution() {
    // control sanity checks:
    if (problem.get_number_of_controls_per_timepoint()
        != init->initial_controls.get_inner_length()) {
      gthrow({"Wrong number of initial values of the controls!"});
    }
    if (not have_same_structure(init->initial_controls, init->lower_bounds)) {
      gthrow({"Number of controls and number of control bounds do not match!"});
    }
    if (not have_same_structure(init->lower_bounds, init->upper_bounds)) {
      gthrow({"The lower and upper bounds do not match!"});
    }
    // constraints sanity checks:
    if (not have_same_structure(
            init->constraint_lower_bounds, init->constraint_upper_bounds)) {
      gthrow(
          {"The lower and upper bounds for the constraints do not "
           "match!"});
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

    // check whether constraint timepoints are a subset of state timepoints and
    // that constraint times start at or after the time at state_timepoints[1]
    auto constraint_iterator = constraint_timepoints.begin();

    // If we are past all constraints and no error was detected, we are clear:
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

  bool IpoptWrapper::get_nlp_info(
      Ipopt::Index &n, Ipopt::Index &m, Ipopt::Index &nnz_jac_g,
      Ipopt::Index &nnz_h_lag, IndexStyleEnum &index_style) {
    n = static_cast<Ipopt::Index>(
        init->initial_controls.get_total_number_of_values());
    m = static_cast<Ipopt::Index>(
        init->constraint_lower_bounds.get_total_number_of_values());

    nnz_jac_g = static_cast<Ipopt::Index>(constraint_jacobian.nonZeros());
    nnz_h_lag = 0; // we don't use the hessian, so we can ignore it
    index_style = Ipopt::TNLP::IndexStyleEnum::C_STYLE; // 0-based indexing.

    // delete initialdata if not needed anymore:
    if (init->obsolete()) {
      init.reset();
    }
    return true;
  }
  bool IpoptWrapper::get_bounds_info(
      Ipopt::Index /*n*/, Ipopt::Number *x_l, Ipopt::Number *x_u,
      Ipopt::Index m, Ipopt::Number *g_l, Ipopt::Number *g_u) {

    assert(m == get_total_no_constraints());

    std::copy(
        init->lower_bounds.get_allvalues().begin(),
        init->lower_bounds.get_allvalues().end(), x_l);
    std::copy(
        init->upper_bounds.get_allvalues().begin(),
        init->upper_bounds.get_allvalues().end(), x_u);
    std::copy(
        init->constraint_lower_bounds.get_allvalues().begin(),
        init->constraint_lower_bounds.get_allvalues().end(), g_l);
    std::copy(
        init->constraint_upper_bounds.get_allvalues().begin(),
        init->constraint_upper_bounds.get_allvalues().end(), g_u);

    // delete initialdata if not needed anymore:
    if (init->obsolete()) {
      init.reset();
    }
    return true;
  }
  bool IpoptWrapper::get_starting_point(
      Ipopt::Index n, bool init_x, Ipopt::Number *x, bool init_z,
      Ipopt::Number * /* z_L */, Ipopt::Number * /* z_U */, Ipopt::Index /*m */,
      bool init_lambda, Ipopt::Number * /* lambda */) {
    assert(init_x);
    assert(not init_z);
    assert(not init_lambda);

    if (not(n == get_total_no_controls())) {
      gthrow(
          {"The number of controls demanded by IPOPT is different from the "
           "number of controls in grazer."});
    }
    // initialize to the given starting point
    Aux::MappedInterpolatingVector ipoptcontrols(
        control_timepoints, controls_per_step(), x, n);
    ipoptcontrols = init->initial_controls;

    // delete initialdata if not needed anymore:
    if (init->obsolete()) {
      init.reset();
    }
    return true;
  }
  bool IpoptWrapper::eval_f(
      Ipopt::Index n, Ipopt::Number const *x, bool new_x,
      Ipopt::Number &objective_value) {
    if (new_x) {
      derivatives_computed = false;
    }
    // Get controls into an InterpolatingVector_Base:
    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), x, n);

    auto *state_pointer
        = cache.compute_states(controls, state_timepoints, initial_state);
    // if the simulation failed: tell the calling site.
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    objective_value = 0;
    // timeindex starts at 1, because at 0 there are initial conditions which
    // can not be altered!
    for (Eigen::Index timeindex = 1; timeindex != states.size(); ++timeindex) {

      objective_value += problem.evaluate_cost(
          state_timepoints[timeindex], states(state_timepoints[timeindex]),
          controls(state_timepoints[timeindex]));
    }
    return true;
  }

  bool IpoptWrapper::eval_grad_f(
      Ipopt::Index n, Ipopt::Number const *x, bool new_x,
      Ipopt::Number *grad_f) {
    if (new_x) {
      derivatives_computed = false;
    }
    // evaluate the jacobian of the constraints function and the objective:
    // If that fails, report failure.
    auto success_derivative_computation = compute_derivatives(new_x, x);
    if (not success_derivative_computation) {
      return false;
    }

    // After the derivatives are computed, we copy them over:
    assert(derivatives_computed);
    Aux::MappedInterpolatingVector grad_f_accessor(
        control_timepoints, controls_per_step(), grad_f, n);
    grad_f_accessor = objective_gradient;
    return true;
  }
  bool IpoptWrapper::eval_g(
      Ipopt::Index n, Ipopt::Number const *x, bool new_x, Ipopt::Index m,
      Ipopt::Number *g) {
    if (new_x) {
      derivatives_computed = false;
    }
    return evaluate_constraints(x, n, g, m);
  }
  bool IpoptWrapper::eval_jac_g(
      Ipopt::Index /*n*/, Ipopt::Number const *x, bool new_x,
      Ipopt::Index /* m */, Ipopt::Index nele_jac, Ipopt::Index *iRow,
      Ipopt::Index *jCol, Ipopt::Number *values) {
    if (new_x) {
      derivatives_computed = false;
    }
    if (values == nullptr) {
      // first internal call of this function.
      // set the structure of constraints jacobian.
      constraint_jacobian.supply_indices(iRow, jCol, nele_jac);
      return true;
    }
    // evaluate the jacobian of the constraints function and the objective:
    // If that fails, report failure.
    auto success_derivative_computation = compute_derivatives(new_x, x);
    if (not success_derivative_computation) {
      return false;
    }

    // After compute_derivatives was called, the jacobian is saved in
    // constraint_jacobian. We now copy it into the ipopt jacobian values.
    assert(derivatives_computed);
    constraint_jacobian_accessor.replace_storage(values, nele_jac);
    constraint_jacobian_accessor = constraint_jacobian;
    constraint_jacobian_accessor.replace_storage(nullptr, nele_jac);

    return true;
  }
  void IpoptWrapper::finalize_solution(
      Ipopt::SolverReturn status, Ipopt::Index n, const Ipopt::Number *x,
      const Ipopt::Number * /* z_L */, const Ipopt::Number * /* z_U */,
      Ipopt::Index /* m */, const Ipopt::Number * /* g */,
      const Ipopt::Number * /* lambda */, Ipopt::Number obj_value,
      const Ipopt::IpoptData * /* ip_data */,
      Ipopt::IpoptCalculatedQuantities * /* ip_cq */) {
    if (status != Ipopt::SUCCESS) {
      std::cout << "Ipopt status was: " << Ipopt::SUCCESS << "\n";
      std::cout << "Compare the values to the enum Ipopt::SUCCESS for what "
                   "that means."
                << std::endl;
      throw std::runtime_error("The solver failed to find a solution");
    }
    Aux::ConstMappedInterpolatingVector const solution_accessor(
        control_timepoints, controls_per_step(), x, n);
    solution = solution_accessor;

    final_objective_value = obj_value;
  }

  bool IpoptWrapper::compute_derivatives(bool new_x, Ipopt::Number const *x) {
    if ((not new_x) and derivatives_computed) {
      return true;
    }

    Aux::ConstMappedInterpolatingVector const controls(
        control_timepoints, controls_per_step(), x, get_total_no_controls());

    auto *state_pointer
        = cache.compute_states(controls, state_timepoints, initial_state);

    // if the simulation failed: tell the calling site.
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    // On the first run we must set the sparsity pattern of the equation
    // derivatives and analyze the pattern inside the LU solver.
    // TODO: Factor into a function.
    if (not equation_matrices_initialized) {
      double last_time = states.interpolation_point_at_index(0);
      double new_time = states.interpolation_point_at_index(1);

      Aux::Triplethandler<Aux::Transposed> new_handler(dE_dnew_transposed);
      problem.d_evalutate_d_new_state(
          new_handler, last_time, new_time, states(last_time), states(new_time),
          controls(new_time));
      new_handler.set_matrix();
      solver.analyzePattern(dE_dnew_transposed);
      Aux::Triplethandler<Aux::Transposed> last_handler(dE_dlast_transposed);
      problem.d_evalutate_d_last_state(
          last_handler, last_time, new_time, states(last_time),
          states(new_time), controls(new_time));
      last_handler.set_matrix();
      Aux::Triplethandler control_handler(dE_dcontrol);
      problem.d_evalutate_d_control(
          control_handler, last_time, new_time, states(last_time),
          states(new_time), controls(new_time));
      control_handler.set_matrix();

      Aux::Triplethandler<Aux::Transposed> gnew_handler(dg_dnew_transposed);
      problem.d_evaluate_constraint_d_state(
          gnew_handler, new_time, states(new_time), controls(new_time));
      gnew_handler.set_matrix();
      Aux::Triplethandler gcontrol_handler(dg_dcontrol);
      problem.d_evaluate_constraint_d_control(
          gcontrol_handler, new_time, states(new_time), controls(new_time));
      gcontrol_handler.set_matrix();

      equation_matrices_initialized = true;
    }
    // First run DONE here.

    // search the last timepoint of a constraint:
    Eigen::Index rev_constraint_index = constraint_steps() - 1;
    Eigen::Index rev_state_index = state_steps() - 1;
    while (constraint_timepoints[rev_constraint_index]
           < state_timepoints[rev_state_index]) {
      assert(rev_state_index > 0);
      --rev_state_index;
    }
    assert(
        constraint_timepoints[rev_constraint_index]
        == state_timepoints[rev_state_index]);

    auto last_time = state_timepoints[rev_state_index - 1];
    auto new_time = state_timepoints[rev_state_index];

    {
      Aux::Coeffrefhandler<Aux::Transposed> new_handler(dE_dnew_transposed);
      problem.d_evalutate_d_new_state(
          new_handler, last_time, new_time, states(last_time), states(new_time),
          controls(new_time));
    }
    solver.factorize(dE_dnew_transposed);

    {
      Aux::Coeffrefhandler<Aux::Transposed> gnew_handler(dg_dnew_transposed);
      problem.d_evaluate_constraint_d_state(
          gnew_handler, new_time, states(new_time), controls(new_time));
    }
    // The dense matrix mostly contains zeros, so we only copy over the entries
    // in the sparse matrix.
    assign_sparse_to_sparse_dense(dg_dnew_dense_transposed, dg_dnew_transposed);

    auto A_block = right_cols(A_jp1_Lambda_j, rev_constraint_index);
    auto Lambda_block = middle_col_block(Lambda_j, rev_constraint_index);
    auto dg_duj_block = middle_row_block(dg_duj, rev_constraint_index);

    A_block.noalias() = dE_dlast_transposed * Lambda_block;
    // finally compute lambda_{nn}:
    Lambda_block.noalias() = solver.solve(dg_dnew_dense_transposed);

    Aux::Coeffrefhandler gcontrol_handler(dg_dcontrol);
    problem.d_evaluate_constraint_d_control(
        gcontrol_handler, new_time, states(new_time), controls(new_time));

    dg_duj_block = (Lambda_block.transpose() * dE_dcontrol) + dg_dcontrol;
    //

    for (; rev_state_index != 0; --rev_state_index) {
      last_time = state_timepoints[rev_state_index - 1];
      new_time = state_timepoints[rev_state_index];
      Aux::Coeffrefhandler<Aux::Transposed> new_handler(dE_dnew_transposed);
      problem.d_evalutate_d_new_state(
          new_handler, last_time, new_time, states(last_time), states(new_time),
          controls(new_time));
      solver.factorize(dE_dnew_transposed);
    }

    derivatives_computed = true;
    return true;
  }

  Aux::InterpolatingVector_Base const &IpoptWrapper::get_solution() const {
    return solution;
  }

  double IpoptWrapper::get_final_objective_value() const {
    return final_objective_value;
  }

  Eigen::Index IpoptWrapper::states_per_step() const {
    return problem.get_number_of_states();
  }
  Eigen::Index IpoptWrapper::controls_per_step() const {
    return problem.get_number_of_controls_per_timepoint();
  }
  Eigen::Index IpoptWrapper::constraints_per_step() const {
    return problem.get_number_of_constraints_per_timepoint();
  }

  Eigen::Index IpoptWrapper::state_steps() const {
    return state_timepoints.size();
  }
  Eigen::Index IpoptWrapper::control_steps() const {
    return control_timepoints.size();
  }
  Eigen::Index IpoptWrapper::constraint_steps() const {
    return constraint_timepoints.size();
  }

  Eigen::Index IpoptWrapper::get_total_no_controls() const {
    return controls_per_step() * control_steps();
  }

  Eigen::Index IpoptWrapper::get_total_no_constraints() const {
    return constraints_per_step() * constraint_steps();
  }

  Eigen::Ref<Eigen::MatrixXd> IpoptWrapper::right_cols(
      Eigen::Ref<Eigen::MatrixXd> Fullmat, Eigen::Index outer_col_index) const {
    assert(Fullmat.rows() == states_per_step());
    assert(Fullmat.cols() == controls_per_step());
    assert(outer_col_index >= 0);
    assert(outer_col_index < constraint_steps());

    return Fullmat.rightCols(
        get_total_no_constraints() - outer_col_index * constraints_per_step());
  }

  Eigen::Ref<Eigen::MatrixXd> IpoptWrapper::middle_col_block(
      Eigen::Ref<Eigen::MatrixXd> Fullmat, Eigen::Index outer_col_index) const {
    assert(Fullmat.rows() == states_per_step());
    assert(Fullmat.cols() == get_total_no_constraints());
    assert(outer_col_index >= 0);
    assert(outer_col_index < constraint_steps());

    return Fullmat.middleCols(
        outer_col_index * constraints_per_step(), constraints_per_step());
  }

  Eigen::Ref<RowMat> IpoptWrapper::lower_rows(
      Eigen::Ref<RowMat> Fullmat, Eigen::Index outer_row_index) const {
    assert(Fullmat.rows() == get_total_no_constraints());
    assert(Fullmat.cols() == controls_per_step());
    assert(outer_row_index >= 0);
    assert(outer_row_index < constraint_steps());

    return Fullmat.bottomRows(
        get_total_no_constraints() - outer_row_index * constraints_per_step());
  }

  Eigen::Ref<RowMat> IpoptWrapper::middle_row_block(
      Eigen::Ref<RowMat> Fullmat, Eigen::Index outer_row_index) const {
    assert(Fullmat.rows() == get_total_no_constraints());
    assert(Fullmat.cols() == controls_per_step());
    assert(outer_row_index >= 0);
    assert(outer_row_index < constraint_steps());
    return Fullmat.middleRows(
        outer_row_index * constraints_per_step(), constraints_per_step());
  }

} // namespace Optimization
