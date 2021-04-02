#include "StochasticPQnode.hpp"
#include "Powernode.hpp"
#include "Stochastic.hpp"
#include "make_schema.hpp"

namespace Model::Networkproblem::Power {

  std::string StochasticPQnode::get_type() { return "StochasticPQnode"; }
  nlohmann::json StochasticPQnode::get_schema() {
    nlohmann::json schema = Powernode::get_schema();
    Aux::schema::add_required(schema, "sigma_P", Aux::schema::type::number());
    Aux::schema::add_required(schema, "theta_P", Aux::schema::type::number());
    Aux::schema::add_required(schema, "sigma_Q", Aux::schema::type::number());
    Aux::schema::add_required(schema, "theta_Q", Aux::schema::type::number());

    return schema;
  }

  StochasticPQnode::StochasticPQnode(nlohmann::json const &topology) :
      Powernode(topology),
      sigma_P(topology["sigma_P"]),
      theta_P(topology["theta_P"]),
      sigma_Q(topology["sigma_Q"]),
      theta_Q(topology["theta_Q"]) {}

  void StochasticPQnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> // last_state
      ,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    auto V_index = get_start_state_index();
    auto phi_index = V_index + 1;
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
    auto first_equation_index = get_start_state_index();
    auto second_equation_index = first_equation_index + 1;
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
