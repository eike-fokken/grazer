#include "StochasticPQnode.hpp"
#include "Stochastic.hpp"

namespace Model::Networkproblem::Power {

  std::string StochasticPQnode::get_type() { return "StochasticPQnode"; }

  void StochasticPQnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {}

  void StochasticPQnode::precompute(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state) {
    auto last_P = last_state(get_start_state_index());
    current_P = Aux::euler_maruyama(
        last_P, theta_P, boundaryvalue(new_time)[0], new_time - last_time,
        sigma_P, distribution);
    auto last_Q = last_state(get_start_state_index() + 1);
    current_Q = Aux::euler_maruyama(
        last_Q, theta_Q, boundaryvalue(new_time)[0], new_time - last_time,
        sigma_Q, distribution);
  }

  void StochasticPQnode::evaluate_state_derivative(
      Aux::Matrixhandler * // jacobianhandler
      ,
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> // last_state
      ,
      Eigen::Ref<Eigen::VectorXd const> // new_state
  ) const {}

  void StochasticPQnode::save_values(
      double time, Eigen::Ref<Eigen::VectorXd const> state) {
    auto P_val = P(state);
    auto Q_val = Q(state);
    save_power_values(time, state, P_val, Q_val);
  }

} // namespace Model::Networkproblem::Power
