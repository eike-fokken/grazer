#include "Powernode.hpp"
#include "Exception.hpp"
#include "Get_base_component.hpp"
#include "InterpolatingVector.hpp"
#include "Matrixhandler.hpp"
#include "Transmissionline.hpp"
#include "make_schema.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <optional>
#include <tuple>
#include <utility>

namespace Model::Power {

  nlohmann::json Powernode::get_schema() {
    nlohmann::json schema = Network::Node::get_schema();
    Aux::schema::add_required(schema, "B", Aux::schema::type::number());
    Aux::schema::add_required(schema, "G", Aux::schema::type::number());

    return schema;
  }

  std::optional<nlohmann::json> Powernode::get_boundary_schema() {
    return Aux::schema::make_boundary_schema(2);
  }

  int Powernode::init_vals_per_interpol_point() { return 2; }

  nlohmann::json Powernode::get_initial_schema() {
    int interpol_points = 1;
    std::vector<nlohmann::json> contains_x
        = {R"({"minimum": 0, "maximum": 0})"_json};
    return Aux::schema::make_initial_schema(
        interpol_points, Powernode::init_vals_per_interpol_point(), contains_x);
  }

  Powernode::Powernode(nlohmann::json const &topology) :
      Node(topology),
      boundaryvalue(Aux::InterpolatingVector::construct_from_json(
          topology["boundary_values"], get_boundary_schema().value())),
      G(topology["G"]),
      B(topology["B"]) {}

  int Powernode::get_number_of_states() const {
    return statemanager.get_number_of_indices();
  }
  int Powernode::get_start_state_index() const {
    return statemanager.get_startindex();
  }
  int Powernode::get_after_state_index() const {
    return statemanager.get_afterindex();
  }

  int Powernode::set_state_indices(int next_free_index) {
    return statemanager.set_indices(next_free_index, number_of_state_variables);
  }

  void Powernode::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) {
    statemanager.set_initial_values(
        new_state, 1, initial_json, get_initial_schema());
  }

  void Powernode::setup() {
    setup_output_json_helper(get_id());

    attached_component_data.clear();

    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      if (!line) {
        continue;
      }
      Powernode *endnode = line->get_ending_powernode();

      double line_G = line->get_G();
      double line_B = line->get_B();

      attached_component_data.push_back({line_G, line_B, endnode});
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      if (!line) {
        continue;
      }
      Powernode *startnode = line->get_starting_powernode();

      double line_G = line->get_G();
      double line_B = line->get_B();

      attached_component_data.push_back({line_G, line_B, startnode});
    }
  }

  double Powernode::get_G() const { return G; }

  double Powernode::get_B() const { return B; }

  void Powernode::add_results_to_json(nlohmann::json &new_output) {
    auto &this_output_json = get_output_json_ref();
    std::string comp_type = Aux::component_class(*this);
    new_output[comp_type][get_power_type()].push_back(
        std::move(this_output_json));
  }

  void Powernode::json_save_power(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state, double P_val,
      double Q_val) {
    nlohmann::json current_value;
    current_value["time"] = time;
    current_value["P"] = P_val;
    current_value["Q"] = Q_val;
    current_value["V"] = state[get_start_state_index()];
    current_value["phi"] = state[get_start_state_index() + 1];

    auto &output_json = get_output_json_ref();
    output_json["data"].push_back(std::move(current_value));
  }

  double Powernode::P(Eigen::Ref<Eigen::VectorXd const> const &state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    double G_i = get_G();

    double V_i = state[V_index];
    double phi_i = state[phi_index];

    double P = 0.0;
    P += G_i * V_i * V_i;
    for (auto &triple : attached_component_data) {
      double G_ik = std::get<0>(triple);
      double B_ik = std::get<1>(triple);
      Powernode *othernode = std::get<2>(triple);
      int V_index_k = othernode->get_start_state_index();
      int phi_index_k = V_index_k + 1;
      double V_k = state[V_index_k];
      double phi_k = state[phi_index_k];
      double phi_ik = phi_i - phi_k;
      P += V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik));
    }
    return P;
  }

  double Powernode::Q(Eigen::Ref<Eigen::VectorXd const> const &state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    double B_i = get_B();
    double V_i = state[V_index];
    double phi_i = state[phi_index];

    double Q = 0.0;
    Q -= B_i * V_i * V_i;
    for (auto &triple : attached_component_data) {

      double G_ik = std::get<0>(triple);
      double B_ik = std::get<1>(triple);
      Powernode *othernode = std::get<2>(triple);
      int V_index_k = othernode->get_start_state_index();
      int phi_index_k = V_index_k + 1;
      double V_k = state[V_index_k];
      double phi_k = state[phi_index_k];
      double phi_ik = phi_i - phi_k;
      Q += V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik));
    }
    return Q;
  }

  void Powernode::evaluate_P_derivative(
      int equationindex, Aux::Matrixhandler &jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    double G_i = get_G();
    double V_i = new_state[V_index];
    double phi_i = new_state[phi_index];

    jacobianhandler.set_coefficient(equationindex, V_index, 2 * G_i * V_i);
    jacobianhandler.set_coefficient(equationindex, phi_index, 0.0);

    for (auto &triple : attached_component_data) {
      double G_ik = std::get<0>(triple);
      double B_ik = std::get<1>(triple);
      Powernode *othernode = std::get<2>(triple);
      int V_index_k = othernode->get_start_state_index();
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler.add_to_coefficient(
          equationindex, V_index,
          V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler.add_to_coefficient(
          equationindex, phi_index,
          V_i * V_k * (-G_ik * sin(phi_ik) + B_ik * cos(phi_ik)));

      jacobianhandler.set_coefficient(
          equationindex, V_index_k,
          V_i * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler.set_coefficient(
          equationindex, phi_index_k,
          V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
    }
  }

  void Powernode::evaluate_Q_derivative(
      int equationindex, Aux::Matrixhandler &jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    double B_i = get_B();
    double V_i = new_state[V_index];
    double phi_i = new_state[phi_index];

    jacobianhandler.set_coefficient(equationindex, V_index, -2 * B_i * V_i);
    jacobianhandler.set_coefficient(equationindex, phi_index, 0.0);

    for (auto &triple : attached_component_data) {
      double G_ik = std::get<0>(triple);
      double B_ik = std::get<1>(triple);
      Powernode *othernode = std::get<2>(triple);
      int V_index_k = othernode->get_start_state_index();
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler.add_to_coefficient(
          equationindex, V_index,
          V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler.add_to_coefficient(
          equationindex, phi_index,
          V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler.set_coefficient(
          equationindex, V_index_k,
          V_i * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler.set_coefficient(
          equationindex, phi_index_k,
          V_i * V_k * (-G_ik * cos(phi_ik) - B_ik * sin(phi_ik)));
    }
  }

} // namespace Model::Power
