#include "StochasticPQnode.hpp"

namespace Model::Networkproblem::Power {

  std::string StochasticPQnode::get_type() { return "StochasticPQnode"; }

  void StochasticPQnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      const Eigen::Ref<const Eigen::VectorXd> &last_state,
      const Eigen::Ref<const Eigen::VectorXd> &new_state) const {}

  void StochasticPQnode::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      const Eigen::Ref<const Eigen::VectorXd> &,
      const Eigen::Ref<const Eigen::VectorXd> &new_state) const {}

  void StochasticPQnode::save_values(
      double time, const Eigen::Ref<const Eigen::VectorXd> &state) {
    auto P_val = P(state);
    auto Q_val = Q(state);
    save_power_values(time, state, P_val, Q_val);
  }
} // namespace Model::Networkproblem::Power
