#include "ExternalPowerplant.hpp"
#include "Gaspowerconnection.hpp"
#include "Matrixhandler.hpp"
#include "Powernode.hpp"
#include "make_schema.hpp"
#include <fstream>

namespace Model::Gaspowerconnection {

  std::string ExternalPowerplant::get_type() { return "ExternalPowerplant"; }
  std::string ExternalPowerplant::get_power_type() const { return get_type(); }

  void ExternalPowerplant::setup() {
    Powernode::setup();
    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Gaspowerconnection *>(start_edge);
      if (!line) {
        continue;
      }
      connection = line;
      return;
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Gaspowerconnection *>(end_edge);
      if (!line) {
        continue;
      }
      connection = line;
      return;
    }
    gthrow(
        {"ExternalPowerplant with id ", get_id(),
         " is not connected to a Gaspowerconnection!\n"});
  }

  void ExternalPowerplant::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

    int V_index = get_startindex();
    int phi_index = V_index + 1;

    if (connection->is_gas_driven(new_time)) {
      // If the gas drives the power output, this node has only one equations.
      rootvalues[phi_index] = new_state[V_index] - boundaryvalue(new_time)[0];
    } else { // is Vphinode!
      rootvalues[V_index] = new_state[V_index] - boundaryvalue(new_time)[0];
      rootvalues[phi_index] = new_state[phi_index] - boundaryvalue(new_time)[1];
    }
  }

  void ExternalPowerplant::d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double /*last_time*/,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const {
    int V_index = get_startindex();
    int phi_index = V_index + 1;

    if (connection->is_gas_driven(new_time)) {
      jacobianhandler.set_coefficient(V_index, V_index, 0.0);
      jacobianhandler.set_coefficient(phi_index, V_index, 1.0);
    } else { // is Vphinode!
      jacobianhandler.set_coefficient(V_index, V_index, 1.0);
      jacobianhandler.set_coefficient(phi_index, phi_index, 1.0);
    }
  }

  void ExternalPowerplant::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
    double P_val = P(state);
    double Q_val = Q(state);
    json_save_power(time, state, P_val, Q_val);
  }

} // namespace Model::Gaspowerconnection
