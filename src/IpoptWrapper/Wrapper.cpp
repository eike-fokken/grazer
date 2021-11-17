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

  Ipopt::Index IpoptWrapper::nnz_constraint_jacobian() {
    return static_cast<Ipopt::Index>(constraint_jacobian.nonZeros());
  }

  void IpoptWrapper::constraint_jacobian_structure(
      Ipopt::Index number_of_nonzeros_in_constraint_jacobian,
      Ipopt::Index *Rows, Ipopt::Index *Cols) {
    if (constraint_jacobian.nonZeros()
        != number_of_nonzeros_in_constraint_jacobian) {
      gthrow({"Wrong number of non-zeros in constraint jacobian!"});
    }
    std::size_t index = 0;
    for (Eigen::Index row = 0; row != constraint_jacobian.rows(); ++row) {
      for (Eigen::Index col = 0; col != constraint_jacobian.size_of_row(row);
           ++col) {
        Rows[index] = static_cast<Ipopt::Index>(row);
        Cols[index] = static_cast<Ipopt::Index>(col);
        ++index;
      }
    }
    if (index
        != static_cast<std::size_t>(
            number_of_nonzeros_in_constraint_jacobian)) {
      gthrow({"Wrong number ofnon-zeros in constraint jacobian!"});
    }
  }

  bool IpoptWrapper::eval_constraint_jacobian(
      Ipopt::Number const *x, Ipopt::Index number_of_controls,
      Ipopt::Number *values, Ipopt::Index nele_jac) {
    Aux::ConstMappedInterpolatingVector const controls(
        initial_controls.get_interpolation_points(),
        initial_controls.get_inner_length(), x,
        static_cast<Eigen::Index>(number_of_controls));

    auto *state_pointer
        = cache.compute_states(controls, simulation_timepoints, initial_state);
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    for (Eigen::Index row = 0; row != constraint_jacobian.rows(); ++row) {
      Eigen::VectorXd ones(constraint_jacobian.size_of_row(row));
      ones.setOnes();
      constraint_jacobian.row(values, nele_jac, row) = ones;
    }
    return true;
  }

  IpoptWrapper::IpoptWrapper(
      Model::Timeevolver &evolver, Model::OptimizableObject &_problem,
      std::vector<double> _simulation_timepoints,
      Eigen::Ref<Eigen::VectorXd const> const &_initial_state,
      Aux::InterpolatingVector_Base const &_initial_controls,
      Aux::InterpolatingVector_Base const &_lower_bounds,
      Aux::InterpolatingVector_Base const &_upper_bounds,
      Aux::InterpolatingVector_Base const &_constraints_lower_bounds,
      Aux::InterpolatingVector_Base const &_constraints_upper_bounds) :
      constraint_jacobian(ConstraintJacobian::make_instance(
          _constraints_lower_bounds, _initial_controls)),
      problem(_problem),
      cache(evolver, _problem),
      simulation_timepoints(_simulation_timepoints),
      initial_state(_initial_state),
      initial_controls(_initial_controls),
      lower_bounds(_lower_bounds),
      upper_bounds(_upper_bounds),
      constraints_lower_bounds(_constraints_lower_bounds),
      constraints_upper_bounds(_constraints_upper_bounds) {

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

    // // final construction:
    // auto triplets = Optimization::constraint_jac_triplets(
    //     constraints_lower_bounds, initial_controls);
    // constraint_jacobian.setFromTriplets(triplets.begin(), triplets.end());
    // constraint_jacobian.makeCompressed();
    assert(false);
  }

  bool IpoptWrapper::get_nlp_info(
      Ipopt::Index &n, Ipopt::Index &m, Ipopt::Index &nnz_jac_g,
      Ipopt::Index &nnz_h_lag, IndexStyleEnum &index_style) {
    n = static_cast<Ipopt::Index>(
        initial_controls.get_total_number_of_values());
    m = static_cast<Ipopt::Index>(
        constraints_lower_bounds.get_total_number_of_values());

    nnz_jac_g = nnz_constraint_jacobian();

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
    std::copy(
        initial_controls.get_allvalues().begin(),
        initial_controls.get_allvalues().end(), x);
    return true;
  }
  bool IpoptWrapper::eval_f(
      Ipopt::Index n, Ipopt::Number const *x, bool /* new_x */,
      Ipopt::Number &obj_value) {
    Aux::ConstMappedInterpolatingVector controls(
        initial_controls.get_interpolation_points(),
        initial_controls.get_inner_length(), x, n);

    auto *state_pointer
        = cache.compute_states(controls, simulation_timepoints, initial_state);
    if (state_pointer == nullptr) {
      return false;
    }
    auto &states = *state_pointer;

    obj_value = 0;
    // timeindex starts at 1, because at 0 there are initial conditions which
    // can not be altered!
    for (Eigen::Index timeindex = 1; timeindex != states.size(); ++timeindex) {
      auto utimeindex = static_cast<size_t>(timeindex);
      obj_value += problem.evaluate_cost(
          simulation_timepoints[utimeindex],
          states(simulation_timepoints[utimeindex]),
          controls(simulation_timepoints[utimeindex]));
    }
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
      Ipopt::Index n, Ipopt::Number const *x, bool /* new_x */,
      Ipopt::Index /* m */, Ipopt::Index nele_jac, Ipopt::Index *iRow,
      Ipopt::Index *jCol, Ipopt::Number *values) {
    if (values == NULL) {
      // first internal call of this function.
      // set the structure of constraints jacobian.
      constraint_jacobian_structure(nele_jac, iRow, jCol);
    } else {
      // evaluate the jacobian of the constraints function
      eval_constraint_jacobian(x, n, values, nele_jac);
    }
    assert(false);
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
      throw std::runtime_error("The solver failed to find a solution");
    }
    solution.resize(n);
    std::copy(x, x + n, solution.begin());
    final_objective_value = obj_value;
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

} // namespace Optimization
