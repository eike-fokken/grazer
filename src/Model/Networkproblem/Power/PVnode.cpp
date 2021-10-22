#include <Matrixhandler.hpp>
#include <PVnode.hpp>
#include <Transmissionline.hpp>
#include <iostream>

namespace Model::Power {

  std::string PVnode::get_type() { return "PVnode"; }
  std::string PVnode::get_power_type() const { return get_type(); }

  void PVnode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    rootvalues[V_index] = P(new_state) - boundaryvalue(new_time)[0];

    rootvalues[phi_index] = new_state[V_index] - boundaryvalue(new_time)[1];
  }

  void PVnode::d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler,
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    auto V_index = get_state_startindex();
    auto phi_index = V_index + 1;
    evaluate_P_derivative(V_index, jacobianhandler, new_state);
    jacobianhandler.set_coefficient(phi_index, V_index, 1.0);
  }

  void PVnode::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
    auto P_val = boundaryvalue(time)[0];
    auto Q_val = Q(state);
    json_save_power(time, state, P_val, Q_val);
  }

} // namespace Model::Power
