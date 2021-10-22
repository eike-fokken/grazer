#include <Matrixhandler.hpp>
#include <PQnode.hpp>
#include <Transmissionline.hpp>
#include <cmath>
#include <iostream>
#include <math.h>

namespace Model::Power {

  std::string PQnode::get_type() { return "PQnode"; }
  std::string PQnode::get_power_type() const { return get_type(); }
  void PQnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    rootvalues[V_index] = P(new_state) - boundaryvalue(new_time)[0];
    rootvalues[phi_index] = Q(new_state) - boundaryvalue(new_time)[1];
  }

  void PQnode::d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler,
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto first_equation_index = get_state_startindex();
    auto second_equation_index = first_equation_index + 1;
    evaluate_P_derivative(first_equation_index, jacobianhandler, new_state);
    evaluate_Q_derivative(second_equation_index, jacobianhandler, new_state);
  }

  void PQnode::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
    auto P_val = boundaryvalue(time)[0];
    auto Q_val = boundaryvalue(time)[1];
    json_save_power(time, state, P_val, Q_val);
  }

} // namespace Model::Power
