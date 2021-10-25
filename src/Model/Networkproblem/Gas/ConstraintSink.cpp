#include "ConstraintSink.hpp"
#include "Matrixhandler.hpp"

namespace Model::Gas {

  void ConstraintSink::evaluate_constraint(
      Eigen::Ref<Eigen::VectorXd> constraint_values, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    auto first_direction = directed_attached_gas_edges[0].first;
    auto sample_gas_edge = directed_attached_gas_edges[0].second;
    auto sample_boundary_state
        = sample_gas_edge->get_boundary_state(first_direction, state);
    auto sample_pressure = sample_boundary_state[0];

    constraint_values[get_start_constraint_index()] = sample_pressure;
  }

  void ConstraintSink::d_evaluate_constraint_d_state(
      Aux::Matrixhandler &constraint_new_state_jacobian_handler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &) const {
    auto first_direction = directed_attached_gas_edges[0].first;
    auto sample_gas_edge = directed_attached_gas_edges[0].second;
    auto index = sample_gas_edge->get_boundary_state_index(first_direction);
    constraint_new_state_jacobian_handler.set_coefficient(
        get_start_constraint_index(), index, 1.0);
  }

  void ConstraintSink::d_evaluate_constraint_d_control(
      Aux::Matrixhandler &, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &) const {}

  void ConstraintSink::set_constraint_lower_bounds(
      Timedata timedata, Eigen::Ref<Eigen::VectorXd> constraint_lower_bounds,
      nlohmann::json const &constraint_lower_bound_json) {
    assert(false); // implement me!
  }

  void ConstraintSink::set_constraint_upper_bounds(
      Timedata timedata, Eigen::Ref<Eigen::VectorXd> constraint_upper_bounds,
      nlohmann::json const &constraint_upper_bound_json) {
    assert(false); // implement me!
  }

  Eigen::Index
  ConstraintSink::needed_number_of_constraints_per_time_point() const {
    return 1;
  }
} // namespace Model::Gas
