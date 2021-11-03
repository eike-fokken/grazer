#include "Wrapper.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Networkproblem.hpp"
#include "Timeevolver.hpp"

namespace Optimization {

  Ipopt::Index IpoptWrapper::nnz_constraint_jacobian() {
    assert(false); // "implement me!"
  }

  void IpoptWrapper::jacobian_structure(
      Ipopt::Index number_of_nonzeros_in_constraint_jacobian,
      Ipopt::Index *Rows, Ipopt::Index *Cols) {

    assert(false); // "implement me!"
  }

  void IpoptWrapper::constraint_jacobian(Ipopt::Number *values) {
    assert(false); // "implement me!"
  }

  IpoptWrapper::IpoptWrapper(
      Model::Timeevolver &evolver, Model::Networkproblem &_problem,
      Model::Timedata _simulation_data, Model::Timedata _controls_data,
      Model::Timedata _constraints_data,
      Eigen::Ref<Eigen::VectorXd const> const &initial_state,
      Aux::InterpolatingVector const &initial_controls,
      Aux::InterpolatingVector const &_lower_bounds,
      Aux::InterpolatingVector const &_upper_bounds,
      Aux::InterpolatingVector const &_constraints_lower_bounds,
      Aux::InterpolatingVector const &_constraints_upper_bounds) :
      problem(_problem),
      cache(evolver, _problem),
      simulation_data(_simulation_data),
      controls_data(_controls_data),
      constraints_data(_constraints_data),
      state_initial_values(initial_state),
      control_initial_values(initial_controls.get_allvalues()),
      lower_bounds(_lower_bounds.get_allvalues()),
      upper_bounds(_upper_bounds.get_allvalues()),
      constraints_lower_bounds(_constraints_lower_bounds.get_allvalues()),
      constraints_upper_bounds(_constraints_upper_bounds.get_allvalues()) {

    // control sanity checks:
    if (problem.get_number_of_controls_per_timepoint()
            * controls_data.get_number_of_time_points()
        != control_initial_values.size()) {
      gthrow({"Wrong number of initial values of the controls!"});
    }
    if (problem.get_number_of_controls_per_timepoint()
            * controls_data.get_number_of_time_points()
        != lower_bounds.size()) {
      gthrow({"Number of controls and number of control bounds do not match!"});
    }
    if (lower_bounds.size() != upper_bounds.size()) {
      gthrow({"The number of lower and upper bounds do not match!"});
    }
    // constraints sanity checks:
    if (constraints_lower_bounds.size() != constraints_upper_bounds.size()) {
      gthrow(
          {"The number of lower and upper bounds for the constraints do not "
           "match!"});
    }
    if (problem.get_number_of_constraints_per_timepoint()
            * constraints_data.get_number_of_time_points()
        != constraints_lower_bounds.size()) {
      gthrow(
          {"Number of constraints and number of constraint bounds do not "
           "match!"});
    }
    // state sanity checks:
    if (problem.get_number_of_states() != state_initial_values.size()) {
      gthrow({"Wrong number of initial values of the states!"});
    }
  }

  bool IpoptWrapper::get_nlp_info(
      Ipopt::Index &n, Ipopt::Index &m, Ipopt::Index &nnz_jac_g,
      Ipopt::Index &nnz_h_lag, IndexStyleEnum &index_style) {
    n = static_cast<Ipopt::Index>(
        problem.get_number_of_controls_per_timepoint()
        * controls_data.get_number_of_time_points());
    m = static_cast<Ipopt::Index>(
        problem.get_number_of_constraints_per_timepoint()
        * constraints_data.get_number_of_time_points());

    nnz_jac_g = nnz_constraint_jacobian();

    nnz_h_lag = 0; // we don't use the hessian, so we can ignore it

    index_style = Ipopt::TNLP::IndexStyleEnum::C_STYLE; // 0-based indexing.
    return true;
  }
  bool IpoptWrapper::get_bounds_info(
      Ipopt::Index /*n*/, Ipopt::Number *x_l, Ipopt::Number *x_u,
      Ipopt::Index /*m*/, Ipopt::Number *g_l, Ipopt::Number *g_u) {
    // all sanity checks have been done in the constructor.
    std::copy(lower_bounds.begin(), lower_bounds.end(), x_l);
    std::copy(upper_bounds.begin(), upper_bounds.end(), x_u);
    std::copy(
        constraints_lower_bounds.begin(), constraints_lower_bounds.end(), g_l);
    std::copy(
        constraints_upper_bounds.begin(), constraints_upper_bounds.end(), g_u);

    return true;
  }
  bool IpoptWrapper::get_starting_point(
      Ipopt::Index /* n */, bool /* init_x */, Ipopt::Number *x,
      bool /* init_z */, Ipopt::Number * /* z_L */, Ipopt::Number * /* z_U */,
      Ipopt::Index /*m */, bool /* init_lambda */,
      Ipopt::Number * /* lambda */) {
    // initialize to the given starting point
    std::copy(control_initial_values.begin(), control_initial_values.end(), x);
    return true;
  }
  bool IpoptWrapper::eval_f(
      Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
      Ipopt::Number &obj_value) {
    Eigen::Map<Eigen::VectorXd const> xx(x, n);
    // static_assert(
    //     false,
    //     "hier weiter, problem macht das nur fuer einen zeitschritt, noch
    //     eine " "abstraction drum rum.");
    // problem.evaluate_cost(
    //     double last_time, double new_time,
    //     const Eigen::Ref<const Eigen::VectorXd> &state,
    //     const Eigen::Ref<const Eigen::VectorXd> &control);
    // obj_value = _objective(xx);
    assert(false);
    return true;
  }
  bool IpoptWrapper::eval_grad_f(
      Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
      Ipopt::Number *grad_f) {
    Eigen::Map<Eigen::VectorXd const> xx(x, n);
    // auto grad = _gradient(xx);
    // std::copy(grad.cbegin(), grad.cend(), grad_f);
    assert(false);
    return true;
  }
  bool IpoptWrapper::eval_g(
      Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */, Ipopt::Index m,
      Ipopt::Number *g) {
    Eigen::Map<Eigen::VectorXd const> xx(x, n);
    // auto constrs = _constraints(xx);
    // assert(constrs.size() == m);
    // std::copy(constrs.cbegin(), constrs.cend(), g);
    assert(false);
    return true;
  }
  bool IpoptWrapper::eval_jac_g(
      Ipopt::Index n, const Ipopt::Number *x, bool /* new_x */,
      Ipopt::Index /* m */, Ipopt::Index /* nele_jac */, Ipopt::Index *iRow,
      Ipopt::Index *jCol, Ipopt::Number *values) {
    if (values == NULL) {
      // first internal call of this function.
      // set the structure of constraints jacobian.
      jacobian_structure(n, iRow, jCol);
    } else {
      // evaluate the jacobian of the constraints function
      // Eigen::VectorXd xx(Eigen::Map<const Eigen::VectorXd>(x, n));
      constraint_jacobian(values);
    }
    assert(false);
    return true;
  }
  void IpoptWrapper::finalize_solution(
      Ipopt::SolverReturn /* status */, Ipopt::Index n, const Ipopt::Number *x,
      const Ipopt::Number * /* z_L */, const Ipopt::Number * /* z_U */,
      Ipopt::Index /* m */, const Ipopt::Number * /* g */,
      const Ipopt::Number * /* lambda */, Ipopt::Number obj_value,
      const Ipopt::IpoptData * /* ip_data */,
      Ipopt::IpoptCalculatedQuantities * /* ip_cq */) {
    // Eigen::VectorXd xx(n);
    // std::copy(x, x + n, xx.begin());
    // solution = xx;
    // final_objective_value = obj_value;
    assert(false);
  }
} // namespace Optimization
