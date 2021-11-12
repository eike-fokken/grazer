#include "Wrapper.hpp"
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

    // loop over the constraint_jacobian and write the indices in the row and
    // column arrays provided by ipopt.
    size_t Ipopt_array_index = 0;
    for (int k = 0; k < constraint_jacobian.outerSize(); ++k)
      for (Eigen::SparseMatrix<double, Eigen::RowMajor>::InnerIterator it(
               constraint_jacobian, k);
           it; ++it) {
        Rows[Ipopt_array_index] = static_cast<Ipopt::Index>(it.row());
        Cols[Ipopt_array_index] = static_cast<Ipopt::Index>(it.col());
        ++Ipopt_array_index;
      }

    assert(Ipopt_array_index == static_cast<size_t>(nnz_constraint_jacobian()));
    assert(
        Ipopt_array_index
        == static_cast<size_t>(number_of_nonzeros_in_constraint_jacobian));
  }

  void IpoptWrapper::eval_constraint_jacobian(Ipopt::Number *values) {

    assert(false);
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
      constraint_jacobian(
          _constraints_lower_bounds.get_total_number_of_values(),
          _initial_controls.get_total_number_of_values()),
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

    // final construction:
    auto triplets = Optimization::constraint_jac_triplets(
        constraints_lower_bounds, initial_controls);
    constraint_jacobian.setFromTriplets(triplets.begin(), triplets.end());
    constraint_jacobian.makeCompressed();
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
      Ipopt::Index /* m */, Ipopt::Index nele_jac, Ipopt::Index *iRow,
      Ipopt::Index *jCol, Ipopt::Number *values) {
    if (values == NULL) {
      // first internal call of this function.
      // set the structure of constraints jacobian.
      constraint_jacobian_structure(nele_jac, iRow, jCol);
    } else {
      // evaluate the jacobian of the constraints function
      eval_constraint_jacobian(values);
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
