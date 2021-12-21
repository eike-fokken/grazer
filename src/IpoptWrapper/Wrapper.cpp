#include "Wrapper.hpp"
#include "ConstraintJacobian.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "OptimizableObject.hpp"
#include "Optimization_helpers.hpp"
#include "Timeevolver.hpp"

#include <IpAlgTypes.hpp>
#include <IpTypes.hpp>
#include <cstddef>
#include <stdexcept>

namespace Optimization {

  bool IpoptWrapper::evaluate_constraints(
      Ipopt::Number const *x, Ipopt::Index number_of_controls,
      Ipopt::Number *values, Ipopt::Index nele_jac) {
    Aux::ConstMappedInterpolatingVector const controls(
        initial_controls.get_interpolation_points(),
        initial_controls.get_inner_length(), x,
        static_cast<Eigen::Index>(number_of_controls));

    // get states:
    auto *state_pointer
        = cache.compute_states(controls, simulation_timepoints, initial_state);
    // if the simulation failed: tell the calling site.
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    Aux::MappedInterpolatingVector constraints(
        constraints_lower_bounds.get_interpolation_points(),
        constraints_lower_bounds.get_inner_length(), values,
        static_cast<Eigen::Index>(nele_jac));

    // fill the constraints vector for every (constraints-)timepoint
    for (Eigen::Index constraints_timeindex = 0;
         constraints_timeindex != constraints.size(); ++constraints_timeindex) {
      double time
          = constraints.interpolation_point_at_index(constraints_timeindex);
      problem.evaluate_constraint(
          constraints.mut_timestep(constraints_timeindex), time, states(time),
          controls(time));
    }
    return true;
  }

  IpoptWrapper::IpoptWrapper(
      Model::Timeevolver &evolver, Model::OptimizableObject &_problem,
      std::vector<double> _simulation_timepoints,
      Eigen::VectorXd _initial_state,
      Aux::InterpolatingVector _initial_controls,
      Aux::InterpolatingVector _lower_bounds,
      Aux::InterpolatingVector _upper_bounds,
      Aux::InterpolatingVector _constraints_lower_bounds,
      Aux::InterpolatingVector _constraints_upper_bounds) :
      objective_gradient(
          _initial_controls.get_interpolation_points(),
          _initial_controls.get_inner_length()),
      constraint_jacobian(
          ConstraintJacobian(_constraints_lower_bounds, _initial_controls)),
      constraint_jacobian_accessor(
          nullptr, constraint_jacobian.nonZeros(), _constraints_lower_bounds,
          _initial_controls),
      Lambda_row_storage(Eigen::MatrixXd::Zero(
          _initial_state.size(),
          _constraints_lower_bounds.get_total_number_of_values())),
      problem(_problem),
      cache(evolver, _problem),
      simulation_timepoints(std::move(_simulation_timepoints)),
      initial_state(std::move(_initial_state)),
      initial_controls(std::move(_initial_controls)),
      lower_bounds(std::move(_lower_bounds)),
      upper_bounds(std::move(_upper_bounds)),
      constraints_lower_bounds(std::move(_constraints_lower_bounds)),
      constraints_upper_bounds(std::move(_constraints_upper_bounds)),
      solution(
          _initial_controls.get_interpolation_points(),
          _initial_controls.get_inner_length()) {

    // control sanity checks:
    if (problem.get_number_of_controls_per_timepoint()
        != initial_controls.get_inner_length()) {
      gthrow({"Wrong number of initial values of the controls!"});
    }
    if (not have_same_structure(initial_controls, lower_bounds)) {
      gthrow({"Number of controls and number of control bounds do not match!"});
    }
    if (not have_same_structure(lower_bounds, upper_bounds)) {
      gthrow({"The lower and upper bounds do not match!"});
    }
    // constraints sanity checks:
    if (not have_same_structure(
            constraints_lower_bounds, constraints_upper_bounds)) {
      gthrow(
          {"The lower and upper bounds for the constraints do not "
           "match!"});
    }
    if (problem.get_number_of_constraints_per_timepoint()
        != constraints_lower_bounds.get_inner_length()) {
      gthrow(
          {"Number of constraints and number of constraint bounds do not "
           "match!"});
    }
    // state sanity checks:
    if (problem.get_number_of_states() != initial_state.size()) {
      gthrow({"Wrong number of initial values of the states!"});
    }

    // To start with a clean slate:
    constraint_jacobian.setZero();
  }

  bool IpoptWrapper::get_nlp_info(
      Ipopt::Index &n, Ipopt::Index &m, Ipopt::Index &nnz_jac_g,
      Ipopt::Index &nnz_h_lag, IndexStyleEnum &index_style) {
    n = static_cast<Ipopt::Index>(
        initial_controls.get_total_number_of_values());
    m = static_cast<Ipopt::Index>(
        constraints_lower_bounds.get_total_number_of_values());

    nnz_jac_g = static_cast<Ipopt::Index>(constraint_jacobian.nonZeros());
    nnz_h_lag = 0; // we don't use the hessian, so we can ignore it
    index_style = Ipopt::TNLP::IndexStyleEnum::C_STYLE; // 0-based indexing.
    return true;
  }
  bool IpoptWrapper::get_bounds_info(
      Ipopt::Index /*n*/, Ipopt::Number *x_l, Ipopt::Number *x_u,
      Ipopt::Index /*m*/, Ipopt::Number *g_l, Ipopt::Number *g_u) {
    // all sanity checks have been done in the constructor.
    std::copy(
        lower_bounds.get_allvalues().begin(),
        lower_bounds.get_allvalues().end(), x_l);
    std::copy(
        upper_bounds.get_allvalues().begin(),
        upper_bounds.get_allvalues().end(), x_u);
    std::copy(
        constraints_lower_bounds.get_allvalues().begin(),
        constraints_lower_bounds.get_allvalues().end(), g_l);
    std::copy(
        constraints_upper_bounds.get_allvalues().begin(),
        constraints_upper_bounds.get_allvalues().end(), g_u);

    return true;
  }
  bool IpoptWrapper::get_starting_point(
      Ipopt::Index n, bool init_x, Ipopt::Number *x, bool init_z,
      Ipopt::Number * /* z_L */, Ipopt::Number * /* z_U */, Ipopt::Index /*m */,
      bool init_lambda, Ipopt::Number * /* lambda */) {

    assert(init_x);
    assert(not init_z);
    assert(not init_lambda);

    if ((not init_x) or init_z or init_lambda) {
      gthrow({"Ipopt demands initialization for non-primal variables!"});
    }

    if (not(n == initial_controls.size())) {
      gthrow(
          {"The number of controls demanded by IPOPT is different from the "
           "number of controls in grazer."});
    }
    // initialize to the given starting point
    Aux::MappedInterpolatingVector ipoptcontrols(
        initial_controls.get_interpolation_points(),
        initial_controls.get_inner_length(), x, n);
    ipoptcontrols = initial_controls;
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
        initial_controls.get_interpolation_points(),
        initial_controls.get_inner_length(), x, n);

    auto *state_pointer
        = cache.compute_states(controls, simulation_timepoints, initial_state);
    // if the simulation failed: tell the calling site.
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    objective_value = 0;
    // timeindex starts at 1, because at 0 there are initial conditions which
    // can not be altered!
    for (Eigen::Index timeindex = 1; timeindex != states.size(); ++timeindex) {
      auto utimeindex = static_cast<size_t>(timeindex);
      objective_value += problem.evaluate_cost(
          simulation_timepoints[utimeindex],
          states(simulation_timepoints[utimeindex]),
          controls(simulation_timepoints[utimeindex]));
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
        initial_controls.get_interpolation_points(),
        initial_controls.get_inner_length(), grad_f, n);
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
        initial_controls.get_interpolation_points(),
        initial_controls.get_inner_length(), x, n);
    solution = solution_accessor;

    final_objective_value = obj_value;
  }

  bool IpoptWrapper::compute_derivatives(bool new_x, Ipopt::Number const *x) {

    if ((not new_x) and derivatives_computed) {
      return true;
    }

    auto number_of_controls = initial_controls.get_total_number_of_values();
    Aux::ConstMappedInterpolatingVector const controls(
        initial_controls.get_interpolation_points(),
        initial_controls.get_inner_length(), x, number_of_controls);

    auto *state_pointer
        = cache.compute_states(controls, simulation_timepoints, initial_state);

    // if the simulation failed: tell the calling site.
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    derivatives_computed = true;
    return true;
  }

  Eigen::Index IpoptWrapper::get_number_of_controls() const {
    return initial_controls.get_total_number_of_values();
  }
  Eigen::Index IpoptWrapper::get_number_of_control_timesteps() const {
    return initial_controls.size();
  }
  Eigen::Index IpoptWrapper::get_number_of_constraint_timesteps() const {
    return constraints_lower_bounds.size();
  }

  Aux::InterpolatingVector_Base const &IpoptWrapper::get_solution() const {
    return solution;
  }

  double IpoptWrapper::get_final_objective_value() const {
    return final_objective_value;
  }
} // namespace Optimization
