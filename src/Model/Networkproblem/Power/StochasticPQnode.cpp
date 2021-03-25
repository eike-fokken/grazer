#include "StochasticPQnode.hpp"

namespace Model::Networkproblem::Power {

  std::string StochasticPQnode::get_type() { return "StochasticPQnode"; }

  void StochasticPQnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {}

  void StochasticPQnode::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {}

  void StochasticPQnode::save_values(
      double time, Eigen::Ref<Eigen::VectorXd const> state) {
    auto P_val = P(state);
    auto Q_val = Q(state);
    save_power_values(time, state, P_val, Q_val);
  }
} // namespace Model::Networkproblem::Power
