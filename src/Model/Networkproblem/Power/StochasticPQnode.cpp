#include "StochasticPQnode.hpp"
#include "Stochastic.hpp"

namespace Model::Networkproblem::Power {

  std::string StochasticPQnode::get_type() { return "StochasticPQnode"; }

  void StochasticPQnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> // last_state
      ,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    rootvalues[V_index] = P(new_state) - current_P;
    rootvalues[phi_index] = Q(new_state) - current_Q;
  }

  void StochasticPQnode::prepare_timestep(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> // new_state
  ) {
    auto last_P = P(last_state);
    current_P = Aux::euler_maruyama(
        last_P, theta_P, boundaryvalue(new_time)[0], new_time - last_time,
        sigma_P, *distribution);
    auto last_Q = Q(last_state);
    current_Q = Aux::euler_maruyama(
        last_Q, theta_Q, boundaryvalue(new_time)[1], new_time - last_time,
        sigma_Q, *distribution);
  }

  void StochasticPQnode::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler,
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int first_equation_index = get_start_state_index();
    int second_equation_index = first_equation_index + 1;
    evaluate_P_derivative(first_equation_index, jacobianhandler, new_state);
    evaluate_Q_derivative(second_equation_index, jacobianhandler, new_state);
  }

  void StochasticPQnode::save_values(
      double time, Eigen::Ref<Eigen::VectorXd const> state) {
    auto P_val = current_P;
    auto Q_val = current_Q;
    save_power_values(time, state, P_val, Q_val);
  }

} // namespace Model::Networkproblem::Power
