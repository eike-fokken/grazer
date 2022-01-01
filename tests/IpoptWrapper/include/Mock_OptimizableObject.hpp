#include "OptimizableObject.hpp"
#include <iostream>

using Costfunction = double(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &);
inline double default_cost(
    Eigen::Ref<Eigen::VectorXd const> const & /*controls*/,
    Eigen::Ref<Eigen::VectorXd const> const & /*states*/) {
  return std::numeric_limits<double>::min();
}

using rootfunction = Eigen::VectorXd(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &);
inline Eigen::VectorXd default_root(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &) {
  return Eigen::VectorXd();
}
using DE_Dnew_function = Eigen::SparseMatrix<double>(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &);
inline Eigen::SparseMatrix<double> default_DE_Dnew(
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &,
    Eigen::Ref<Eigen::VectorXd const> const &) {
  return Eigen::SparseMatrix<double>();
}

class Mock_OptimizableObject final : public Model::OptimizableObject {
public:
  Eigen::Index const number_of_states;
  Eigen::Index const number_of_controls;
  Eigen::Index const number_of_constraints;

  rootfunction *E;
  DE_Dnew_function *dE_dnew;

  Costfunction *f;

  Mock_OptimizableObject(
      Eigen::Index _number_of_states, Eigen::Index _number_of_controls,
      Eigen::Index _number_of_constraints, rootfunction *_E = default_root,
      DE_Dnew_function *_dE_dnew = default_DE_Dnew,
      Costfunction *_f = default_cost) :
      number_of_states(_number_of_states),
      number_of_controls(_number_of_controls),
      number_of_constraints(_number_of_constraints),
      E(_E),
      dE_dnew(_dE_dnew),
      f(_f) {}

  void setup() final {
    assert(false); // never call me!
  };
  /// constraint component overrides:

  void evaluate_constraint(
      Eigen::Ref<Eigen::VectorXd> constraint_values, double time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false); // never call me!
  }

  void d_evaluate_constraint_d_state(
      Aux::Matrixhandler &constraint_new_state_jacobian_handler, double time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false); // never call me!
  }

  void d_evaluate_constraint_d_control(
      Aux::Matrixhandler &constraint_control_jacobian_handler, double time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false); // never call me!
  }

  Eigen::Index set_constraint_indices(Eigen::Index next_index) final {
    constraint_startindex = next_index;
    constraint_afterindex = constraint_startindex + number_of_constraints;
    return constraint_afterindex;
  }

  // not needed:
  void set_constraint_lower_bounds(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {
    assert(false); // never call me!
  }

  void set_constraint_upper_bounds(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {
    assert(false); // never call me!
  }

  //// control components:

  void evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false); // never call me!
  }

  void prepare_timestep(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) final {
    std::cout << "Called prepare_timestep!" << std::endl;
  }

  void d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false); // never call me!
  }

  void d_evalutate_d_last_state(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false); // never call me!
  }

  void d_evalutate_d_control(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false); // never call me!
  }

  Eigen::Index set_control_indices(Eigen::Index next_index) final {
    control_startindex = next_index;
    control_afterindex = control_startindex + number_of_controls;
    return control_afterindex;
  }

  // not needed:
  void set_initial_controls(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {
    assert(false); // never call me!
  }

  void set_lower_bounds(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {
    assert(false); // never call me!
  }

  void set_upper_bounds(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {
    assert(false); // never call me!
  }

  //// Cost components:
  double evaluate_cost(
      double /*new_time*/, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    return f(control, state);
  }

  void d_evaluate_cost_d_state(
      Aux::Matrixhandler &cost_new_state_jacobian_handler, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false); // never call me!
  }

  void d_evaluate_cost_d_control(
      Aux::Matrixhandler &cost_control_jacobian_handler, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false); // never call me!
  }

  //// State components:

  Eigen::Index set_state_indices(Eigen::Index next_index) final {
    state_startindex = next_index;
    state_afterindex = state_startindex + number_of_states;
    return state_afterindex;
  }

  void add_results_to_json(nlohmann::json &) final {
    assert(false); // never call me!
  }

  void
  json_save(double time, Eigen::Ref<Eigen::VectorXd const> const &state) final {
  }

  // unneeded:
  void set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) const final {
    assert(false); // never call me!
  }
};
