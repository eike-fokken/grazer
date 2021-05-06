#include <Matrixhandler.hpp>
#include <PQnode.hpp>
#include <Transmissionline.hpp>
#include <cmath>
#include <iostream>
#include <math.h>

namespace Model::Networkproblem::Power {

  std::string PQnode::get_type() { return "PQnode"; }
  std::string PQnode::get_power_type() const { return get_type(); }
  void PQnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    rootvalues[V_index] = P(new_state) - boundaryvalue(new_time)[0];
    rootvalues[phi_index] = Q(new_state) - boundaryvalue(new_time)[1];
  }

  void PQnode::evaluate_state_derivative(
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

  void PQnode::json_save(double time, Eigen::Ref<Eigen::VectorXd const> state) {
    auto P_val = boundaryvalue(time)[0];
    auto Q_val = boundaryvalue(time)[1];
    json_save_power(time, state, P_val, Q_val);
  }

} // namespace Model::Networkproblem::Power
