#include "SwitchedPowerplant.hpp"
#include "Powernode.hpp"
#include "make_schema.hpp"
#include <fstream>

namespace Model::Networkproblem::Power {

  std::string SwitchedPowerplant::get_type() { return "SwitchedPowerplant"; }
  std::string SwitchedPowerplant::get_power_type() const { return get_type(); }

  nlohmann::json SwitchedPowerplant::get_schema() {
    nlohmann::json schema = Powernode::get_schema();
    // Hier anders !
    Aux::schema::add_required(schema, "sigma_P", Aux::schema::type::number());
    Aux::schema::add_required(schema, "theta_P", Aux::schema::type::number());
    Aux::schema::add_required(schema, "sigma_Q", Aux::schema::type::number());
    Aux::schema::add_required(schema, "theta_Q", Aux::schema::type::number());

    return schema;
  }

  SwitchedPowerplant::SwitchedPowerplant(nlohmann::json const &topology) :
      Powernode(topology),
      control(topology["control_values"]),
      PVboundaryvalues(topology["pvboundary_values"]) {}

  void SwitchedPowerplant::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> // last_state
      ,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    // auto V_index = get_start_state_index();
    // auto phi_index = V_index + 1;
    // rootvalues[V_index] = P(new_state) - current_P;
    // rootvalues[phi_index] = Q(new_state) - current_Q;
  }

  void SwitchedPowerplant::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler,
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {}

  void SwitchedPowerplant::save_values(
      double time, Eigen::Ref<Eigen::VectorXd const> state) {

    // auto P_val = current_P;
    // auto Q_val = current_Q;
    // auto P_deviation = P_val - boundaryvalue(time)[0];
    // auto Q_deviation = Q_val - boundaryvalue(time)[1];
    // std::map<double, double> Pmap;
    // std::map<double, double> Qmap;
    // std::map<double, double> Vmap;
    // std::map<double, double> phimap;
    // std::map<double, double> P_deviation_map;
    // std::map<double, double> Q_deviation_map;

    // std::vector<std::map<double, double>> value_vector;
    // Pmap = {{0.0, P_val}};
    // Qmap = {{0.0, Q_val}};
    // Vmap = {{0.0, state[get_start_state_index()]}};
    // phimap = {{0.0, state[get_start_state_index() + 1]}};
    // P_deviation_map = {{0.0, P_deviation}};
    // Q_deviation_map = {{0.0, Q_deviation}};
    // value_vector = {Pmap, Qmap, Vmap, phimap, P_deviation_map,
    // Q_deviation_map}; Equationcomponent::push_to_values(time, value_vector);
  }

  void SwitchedPowerplant::json_save(
      nlohmann::json &output, double time,
      Eigen::Ref<Eigen::VectorXd const> state) const {
    // auto P_val = current_P;
    // auto Q_val = current_Q;
    // auto P_deviation = P_val - boundaryvalue(time)[0];
    // auto Q_deviation = Q_val - boundaryvalue(time)[1];

    // nlohmann::json &current_component_vector = output[get_power_type()];

    // nlohmann::json current_value;
    // current_value["time"] = time;
    // current_value["P"] = P_val;
    // current_value["Q"] = Q_val;
    // current_value["V"] = state[get_start_state_index()];
    // current_value["phi"] = state[get_start_state_index() + 1];
    // current_value["P_deviation"] = P_deviation;
    // current_value["Q_deviation"] = Q_deviation;

    // auto id = get_id_copy();

    // // define a < function as a lambda:
    // auto id_compare_less
    //     = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
    //   return a["id"].get<std::string>() < b["id"].get<std::string>();
    // };

    // // auto id_is = [id](nlohmann::json const &a) -> bool {
    // //   return a["id"].get<std::string>() == id;
    // // };
    // nlohmann::json this_id;
    // this_id["id"] = id;
    // auto it = std::lower_bound(
    //     current_component_vector.begin(), current_component_vector.end(),
    //     this_id, id_compare_less);

    // if (it == current_component_vector.end()) {
    //   // std::cout << "Not found!" << std::endl;
    //   nlohmann::json newoutput;
    //   newoutput["id"] = get_id_copy();
    //   newoutput["data"] = nlohmann::json::array();
    //   newoutput["data"].push_back(current_value);
    //   current_component_vector.push_back(newoutput);
    // } else {
    //   if ((*it)["id"] != id) {
    //     gthrow(
    //         {"The json value\n", (*it).dump(4),
    //          "\n has an id different from the current object, whose id is ",
    //          id,
    //          "\n This is a bug. Please report it!"});
    //   }
    //   nlohmann::json &outputjson = (*it);
    //   outputjson["data"].push_back(current_value);
    // }
  }

} // namespace Model::Networkproblem::Power
