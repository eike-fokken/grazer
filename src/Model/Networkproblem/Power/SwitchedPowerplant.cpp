#include "SwitchedPowerplant.hpp"
#include "Matrixhandler.hpp"
#include "Powernode.hpp"
#include "make_schema.hpp"
#include <fstream>

namespace Model::Networkproblem::Power {

  std::string SwitchedPowerplant::get_type() { return "SwitchedPowerplant"; }
  std::string SwitchedPowerplant::get_power_type() const { return get_type(); }

  nlohmann::json SwitchedPowerplant::get_schema() {
    nlohmann::json schema = Powernode::get_schema();

    Aux::schema::add_property(
        schema, "boundary_values_pv", Aux::schema::make_boundary_schema(2));
    Aux::schema::add_property(
        schema, "control_values", Aux::schema::make_boundary_schema(1));

    return schema;
  }

  SwitchedPowerplant::SwitchedPowerplant(nlohmann::json const &topology) :
      Powernode(topology),
      is_PV_node(topology["control_values"]),
      boundary_values_pv(topology["boundary_values_pv"]) {}

  void SwitchedPowerplant::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {

    int V_index = get_start_state_index();
    int phi_index = V_index + 1;

    // using doubles as bools is a little dirty...
    if (is_PV_node(new_time)[0] != 0.0) {
      rootvalues[V_index] = P(new_state) - boundaryvalue(new_time)[0];
      rootvalues[phi_index] = new_state[V_index] - boundaryvalue(new_time)[1];
    } else { // is Vphinode!
      rootvalues[V_index] = new_state[V_index] - boundaryvalue(new_time)[0];
      rootvalues[phi_index] = new_state[phi_index] - boundaryvalue(new_time)[1];
    }
  }

  void SwitchedPowerplant::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler,
      double // last_time
      ,
      double new_time, Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;

    if (is_PV_node(new_time)[0] != 0.0) {
      evaluate_P_derivative(V_index, jacobianhandler, new_state);
      jacobianhandler->set_coefficient(phi_index, V_index, 1.0);
    } else { // is Vphinode!
      jacobianhandler->set_coefficient(V_index, V_index, 1.0);
      jacobianhandler->set_coefficient(phi_index, phi_index, 1.0);
    }
  }

  void SwitchedPowerplant::save_values(
      double time, Eigen::Ref<Eigen::VectorXd const> state) {
    double P_val;
    double Q_val;

    if (is_PV_node(time)[0] != 0.0) {
      P_val = boundaryvalue(time)[0];
      Q_val = Q(state);
    } else { // Vphinode
      P_val = P(state);
      Q_val = Q(state);
    }
    save_power_values(time, state, P_val, Q_val);
  }

  void SwitchedPowerplant::json_save(
      nlohmann::json &output, double time,
      Eigen::Ref<Eigen::VectorXd const> state) const {
    double P_val;
    double Q_val;

    if (is_PV_node(time)[0] != 0.0) {
      P_val = boundaryvalue(time)[0];
      Q_val = Q(state);
    } else { // Vphinode
      P_val = P(state);
      Q_val = Q(state);
    }
    json_save_power(output, time, state, P_val, Q_val);
  }

} // namespace Model::Networkproblem::Power
