#include "OptimizableObject.hpp"

class TestProblem final : public Model::OptimizableObject {

  void setup() final{};
  /// constraint component overrides:

  void evaluate_constraint(
      Eigen::Ref<Eigen::VectorXd> constraint_values, double time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {}

  void d_evaluate_constraint_d_state(
      Aux::Matrixhandler &constraint_new_state_jacobian_handler, double time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {}

  void d_evaluate_constraint_d_control(
      Aux::Matrixhandler &constraint_control_jacobian_handler, double time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {}

  Eigen::Index set_constraint_indices(Eigen::Index) final { return -1; }

  // not needed:
  void set_constraint_lower_bounds(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {}

  void set_constraint_upper_bounds(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {}

  //// control components:

  void evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {}

  void prepare_timestep(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) final {}

  void d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {}

  void d_evalutate_d_last_state(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {}

  void d_evalutate_d_control(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {}

  Eigen::Index set_control_indices(Eigen::Index) final { return -1; }

  // not needed:
  void set_initial_controls(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {}

  void set_lower_bounds(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {}

  void set_upper_bounds(
      Aux::InterpolatingVector_Base &, nlohmann::json const &) const final {}

  //// Cost components:
  double evaluate_cost(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {
    assert(false);
    return -1;
  }

  void d_evaluate_cost_d_state(
      Aux::Matrixhandler &cost_new_state_jacobian_handler, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {}

  void d_evaluate_cost_d_control(
      Aux::Matrixhandler &cost_control_jacobian_handler, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const final {}

  //// State components:

  Eigen::Index set_state_indices(Eigen::Index) final {
    state_startindex = 3;
    return -1;
  }

  void add_results_to_json(nlohmann::json &) final {}

  void
  json_save(double time, Eigen::Ref<Eigen::VectorXd const> const &state) final {
  }

  // unneeded:
  void set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) final {}
};
