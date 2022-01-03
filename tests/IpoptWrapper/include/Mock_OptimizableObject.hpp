#include "Matrixhandler.hpp"
#include "OptimizableObject.hpp"
#include <Eigen/src/SparseCore/SparseUtil.h>
#include <cstddef>
#include <iostream>

using Costfunction = double(
    Eigen::Ref<Eigen::VectorXd const> const & /*controls*/,
    Eigen::Ref<Eigen::VectorXd const> const & /*states*/);
inline double default_cost(
    Eigen::Ref<Eigen::VectorXd const> const &controls,
    Eigen::Ref<Eigen::VectorXd const> const &states) {
  return controls.norm() + 3 * states.norm();
}

using Dcost_Dnew_function = Eigen::SparseMatrix<double>(
    Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
    Eigen::Ref<Eigen::VectorXd const> const & /*controls*/);
inline Eigen::SparseMatrix<double> default_Dcost_Dnew(
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const & /*controls*/) {
  Eigen::SparseMatrix<double> derivative(1, new_state.size());
  std::vector<Eigen::Triplet<double>> triplets(
      static_cast<size_t>(new_state.size()));
  for (int i = 0; i != static_cast<Eigen::Index>(triplets.size()); ++i) {
    triplets.push_back({0, i, 2 * new_state(i)});
  }
  derivative.setFromTriplets(triplets.begin(), triplets.end());
  return 3 * derivative;
}

using Dcost_Dcontrol_function = Eigen::SparseMatrix<double>(
    Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
    Eigen::Ref<Eigen::VectorXd const> const & /*controls*/);
inline Eigen::SparseMatrix<double> default_Dcost_Dcontrol(
    Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
    Eigen::Ref<Eigen::VectorXd const> const &controls) {
  Eigen::SparseMatrix<double> derivative(1, controls.size());
  std::vector<Eigen::Triplet<double>> triplets(
      static_cast<size_t>(controls.size()));
  for (int i = 0; i != static_cast<Eigen::Index>(triplets.size()); ++i) {
    triplets.push_back({0, i, 2 * controls(i)});
  }
  derivative.setFromTriplets(triplets.begin(), triplets.end());
  return derivative;
}

using equation_function = Eigen::VectorXd(
    Eigen::Ref<Eigen::VectorXd const> const &last_state,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const &controls);
inline Eigen::VectorXd default_equation_function(
    Eigen::Ref<Eigen::VectorXd const> const &last_state,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const &controls) {
  assert(new_state.size() == controls.size());
  assert(last_state.size() == controls.size());
  Eigen::VectorXd equations(new_state.size());
  equations = new_state - 2 * last_state + 3 * controls;
  return equations;
}
using DE_Dnew_function = Eigen::SparseMatrix<double>(
    Eigen::Ref<Eigen::VectorXd const> const &last_state,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const &controls);
inline Eigen::SparseMatrix<double> default_DE_Dnew(
    Eigen::Ref<Eigen::VectorXd const> const &last_state,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const &controls) {
  assert(new_state.size() == controls.size());
  assert(last_state.size() == controls.size());

  Eigen::SparseMatrix<double> Derivative(new_state.size(), new_state.size());
  Derivative.setIdentity();
  return Derivative;
}

using DE_Dlast_function = Eigen::SparseMatrix<double>(
    Eigen::Ref<Eigen::VectorXd const> const &last_state,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const &controls);
inline Eigen::SparseMatrix<double> default_DE_Dlast(
    Eigen::Ref<Eigen::VectorXd const> const &last_state,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const &controls) {
  assert(new_state.size() == controls.size());
  assert(last_state.size() == controls.size());
  Eigen::SparseMatrix<double> Derivative(new_state.size(), last_state.size());
  Derivative.setIdentity();
  return -2 * Derivative;
}

using DE_Dcontrol_function = Eigen::SparseMatrix<double>(
    Eigen::Ref<Eigen::VectorXd const> const &last_state,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const &controls);
inline Eigen::SparseMatrix<double> default_DE_Dcontrol(
    Eigen::Ref<Eigen::VectorXd const> const &last_state,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const &controls) {
  assert(new_state.size() == controls.size());
  assert(last_state.size() == controls.size());
  Eigen::SparseMatrix<double> Derivative(new_state.size(), controls.size());
  Derivative.setIdentity();
  return 3 * Derivative;
}

//////////////////////////////////////////////////////////////////////////////
// constraints:
//////////////////////////////////////////////////////////////////////////////

using constraintfunction = Eigen::VectorXd(
    Eigen::Index number_of_constraints,
    Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
    Eigen::Ref<Eigen::VectorXd const> const & /*controls*/);
inline Eigen::VectorXd default_constraint(
    Eigen::Index number_of_constraints,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const &controls) {

  Eigen::VectorXd constraints(number_of_constraints);
  for (Eigen::Index i = 0; i != constraints.size(); ++i) {
    constraints(i)
        = static_cast<double>(i + 1) * (controls.norm() + 3 * new_state.norm());
  }
  return constraints;
}
using Dconstraint_Dnew_function = Eigen::SparseMatrix<double>(
    Eigen::Index number_of_constraints,
    Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
    Eigen::Ref<Eigen::VectorXd const> const & /*controls*/);
inline Eigen::SparseMatrix<double> default_Dconstraint_Dnew(
    Eigen::Index number_of_constraints,
    Eigen::Ref<Eigen::VectorXd const> const &new_state,
    Eigen::Ref<Eigen::VectorXd const> const & /*controls*/) {
  return Eigen::SparseMatrix<double>(number_of_constraints, new_state.size());
}

using Dconstraint_Dcontrol_function = Eigen::SparseMatrix<double>(
    Eigen::Index number_of_constraints,
    Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
    Eigen::Ref<Eigen::VectorXd const> const & /*controls*/);
inline Eigen::SparseMatrix<double> default_Dconstraint_Dcontrol(
    Eigen::Index number_of_constraints,
    Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
    Eigen::Ref<Eigen::VectorXd const> const &controls) {
  return Eigen::SparseMatrix<double>(number_of_constraints, controls.size());
}
///////////////////////////////////////////////////////////////////////////

class Mock_OptimizableObject final : public Model::OptimizableObject {
public:
  Eigen::Index const number_of_states;
  Eigen::Index const number_of_controls;
  Eigen::Index const number_of_constraints;

  equation_function *E;
  DE_Dnew_function *dE_dnew;
  DE_Dlast_function *dE_dlast;
  DE_Dcontrol_function *dE_dcontrol;

  Costfunction *cost;
  Dcost_Dnew_function *dcost_dnew;
  Dcost_Dcontrol_function *dcost_dcontrol;

  constraintfunction *constraints;
  Dconstraint_Dnew_function *dconstraint_dnew;
  Dconstraint_Dcontrol_function *dconstraint_dcontrol;

  Mock_OptimizableObject(
      Eigen::Index _number_of_states, Eigen::Index _number_of_controls,
      Eigen::Index _number_of_constraints,
      equation_function *_E = default_equation_function,
      DE_Dnew_function *_dE_dnew = default_DE_Dnew,
      DE_Dlast_function *_dE_dlast = default_DE_Dlast,
      DE_Dcontrol_function *_dE_dcontrol = default_DE_Dcontrol,
      Costfunction *_cost = default_cost,
      Dcost_Dnew_function *_dcost_dnew = default_Dcost_Dcontrol,
      Dcost_Dcontrol_function *_dcost_dcontrol = default_Dcost_Dcontrol,
      constraintfunction *_constraints = default_constraint,
      Dconstraint_Dnew_function *_dconstraint_dnew
      = default_Dconstraint_Dcontrol,
      Dconstraint_Dcontrol_function *_dconstraint_dcontrol
      = default_Dconstraint_Dcontrol) :
      number_of_states(_number_of_states),
      number_of_controls(_number_of_controls),
      number_of_constraints(_number_of_constraints),
      E(_E),
      dE_dnew(_dE_dnew),
      dE_dlast(_dE_dlast),
      dE_dcontrol(_dE_dcontrol),
      cost(_cost),
      dcost_dnew(_dcost_dnew),
      dcost_dcontrol(_dcost_dcontrol),
      constraints(_constraints),
      dconstraint_dnew(_dconstraint_dnew),
      dconstraint_dcontrol(_dconstraint_dcontrol) {}

  void setup() final {
    assert(false); // never call me!
  };
  /// constraint component overrides:

  void evaluate_constraint(
      Eigen::Ref<Eigen::VectorXd> constraint_values, double /*time*/,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    constraint_values = constraints(number_of_constraints, state, control);
  }

  void d_evaluate_constraint_d_state(
      Aux::Matrixhandler &constraint_new_state_jacobian_handler,
      double /*time*/, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    auto mat = dconstraint_dnew(number_of_constraints, state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        constraint_new_state_jacobian_handler.set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  }

  void d_evaluate_constraint_d_control(
      Aux::Matrixhandler &constraint_control_jacobian_handler, double /*time*/,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    auto mat = dconstraint_dcontrol(number_of_constraints, state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        constraint_control_jacobian_handler.set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
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
      Eigen::Ref<Eigen::VectorXd> rootvalues, double /*last_time*/,
      double /*new_time*/, Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    rootvalues = E(last_state, new_state, control);
  }

  void prepare_timestep(
      double /*last_time*/, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*control*/) final {
    // std::cout << "Called prepare_timestep!" << std::endl;
  }

  void d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double /*last_time*/,
      double /*new_time*/, Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    auto mat = dE_dnew(last_state, new_state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        jacobianhandler.set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  }

  void d_evalutate_d_last_state(
      Aux::Matrixhandler &jacobianhandler, double /*last_time*/,
      double /*new_time*/, Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    auto mat = dE_dlast(last_state, new_state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        jacobianhandler.set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  }

  void d_evalutate_d_control(
      Aux::Matrixhandler &jacobianhandler, double /*last_time*/,
      double /*new_time*/, Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    auto mat = dE_dcontrol(last_state, new_state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        jacobianhandler.set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
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
    return cost(control, state);
  }

  void d_evaluate_cost_d_state(
      Aux::Matrixhandler &cost_new_state_jacobian_handler, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    auto mat = dcost_dnew(state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        cost_new_state_jacobian_handler.set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
  }

  void d_evaluate_cost_d_control(
      Aux::Matrixhandler &cost_control_jacobian_handler, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    auto mat = dcost_dcontrol(state, control);
    for (int k = 0; k < mat.outerSize(); ++k)
      for (Eigen::SparseMatrix<double>::InnerIterator it(mat, k); it; ++it) {
        cost_control_jacobian_handler.set_coefficient(
            static_cast<int>(it.row()), static_cast<int>(it.col()), it.value());
      }
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

  void json_save(
      double /*time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*state*/) final {}

  // unneeded:
  void set_initial_values(
      Eigen::Ref<Eigen::VectorXd> /*new_state*/,
      nlohmann::json const & /*initial_json*/) const final {
    assert(false); // never call me!
  }
};
