#include "Powernode.hpp"
#include "Boundaryvalue.hpp"
#include "Exception.hpp"
#include "Initialvalue.hpp"
#include "Matrixhandler.hpp"
#include "Transmissionline.hpp"
#include "make_schema.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <tuple>
#include <utility>

namespace Model::Networkproblem::Power {

  nlohmann::json Powernode::get_schema() {
    nlohmann::json schema = Network::Node::get_schema();
    Aux::schema::add_required(schema, "B", Aux::schema::type::number());
    Aux::schema::add_required(schema, "G", Aux::schema::type::number());

    Aux::schema::add_required(
        schema, "boundary_values", Aux::schema::make_boundary_schema(2));
    return schema;
  }

  int Powernode::get_dimension_of_pde() { return 4; }

  nlohmann::json Powernode::get_initial_schema() {
    int interpol_points = 1;
    std::vector<nlohmann::json> contains_x
        = {R"({"minimum": 0, "maximum": 0})"_json};
    return Aux::schema::make_initial_schema(
        interpol_points, Powernode::get_dimension_of_pde(), contains_x);
  }

  Powernode::Powernode(nlohmann::json const &topology) :
      Node(topology),
      boundaryvalue(topology["boundary_values"]),
      G(topology["G"]),
      B(topology["B"]) {}

  void Powernode::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) {

    if (get_start_state_index() == -1) {
      gthrow(
          {"This function may only be called if set_indices  has been "
           "called beforehand!"});
    }

    Initialvalue<4> initialvalues(initial_json);
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    new_state[V_index] = initialvalues(0)[2];
    new_state[phi_index] = initialvalues(0)[3];
  }

  void Powernode::setup() {
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

  int Powernode::get_number_of_states() const {
    return number_of_state_variables;
  }

  // int Powernode::get_number_of_printout_states() const { return 4;}

  double Powernode::get_G() const { return G; }

  double Powernode::get_B() const { return B; }

  void
  Powernode::print_to_files(std::filesystem::path const &output_directory) {

    auto node_output_file = output_directory
                            / std::filesystem::path(get_power_type())
                            / std::filesystem::path(get_id_copy());

    std::ofstream output(node_output_file);

    output << "time,    P,    Q,    V,    phi\n";
    auto values = get_values();
    output.precision(9);
    auto times = get_times();

    for (unsigned long i = 0; i != times.size(); ++i) {
      output << times[i];
      for (auto const &var : values[i]) { output << ",    " << var.at(0.0); }
      output << "\n";
    }
  }

  void Powernode::save_power_values(
      double time, Eigen::Ref<Eigen::VectorXd const> state, double P_val,
      double Q_val) {

    std::map<double, double> Pmap;
    std::map<double, double> Qmap;
    std::map<double, double> Vmap;
    std::map<double, double> phimap;
    std::vector<std::map<double, double>> value_vector;
    Pmap = {{0.0, P_val}};
    Qmap = {{0.0, Q_val}};
    Vmap = {{0.0, state[get_start_state_index()]}};
    phimap = {{0.0, state[get_start_state_index() + 1]}};
    value_vector = {Pmap, Qmap, Vmap, phimap};
    Equationcomponent::push_to_values(time, value_vector);
  }

  void Powernode::json_save_power(
      nlohmann::json &output, double time,
      Eigen::Ref<Eigen::VectorXd const> state, double P_val,
      double Q_val) const {

    nlohmann::json &current_component_vector = output[get_power_type()];

    nlohmann::json current_value;
    current_value["time"] = time;
    current_value["P"] = P_val;
    current_value["Q"] = Q_val;
    current_value["V"] = state[get_start_state_index()];
    current_value["phi"] = state[get_start_state_index() + 1];

    auto id = get_id_copy();
    // define a < function as a lambda:
    auto id_compare_less
        = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
      return a["id"].get<std::string>() < b["id"].get<std::string>();
    };

    // auto id_is = [id](nlohmann::json const &a) -> bool {
    //   return a["id"].get<std::string>() == id;
    // };
    nlohmann::json this_id;
    this_id["id"] = id;
    auto it = std::lower_bound(
        current_component_vector.begin(), current_component_vector.end(),
        this_id, id_compare_less);

    if (it == current_component_vector.end()) {
      // std::cout << "Not found!" << std::endl;
      nlohmann::json newoutput;
      newoutput["id"] = get_id_copy();
      newoutput["data"] = nlohmann::json::array();
      newoutput["data"].push_back(current_value);
      current_component_vector.push_back(newoutput);
    } else {
      if ((*it)["id"] != id) {
        gthrow(
            {"The json value\n", (*it).dump(4),
             "\n has an id different from the current object, whose id is ", id,
             "\n This is a bug. Please report it!"});
      }
      // std::cout << "found!" << std::endl;
      nlohmann::json &outputjson = (*it);
      outputjson["data"].push_back(current_value);
    }
  }

  double Powernode::P(Eigen::Ref<Eigen::VectorXd const> state) const {
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

  double Powernode::Q(Eigen::Ref<Eigen::VectorXd const> state) const {

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
      int equationindex, Aux::Matrixhandler *jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    double G_i = get_G();
    double V_i = new_state[V_index];
    double phi_i = new_state[phi_index];

    jacobianhandler->set_coefficient(equationindex, V_index, 2 * G_i * V_i);
    jacobianhandler->set_coefficient(equationindex, phi_index, 0.0);

    for (auto &triple : attached_component_data) {
      double G_ik = std::get<0>(triple);
      double B_ik = std::get<1>(triple);
      Powernode *othernode = std::get<2>(triple);
      int V_index_k = othernode->get_start_state_index();
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler->add_to_coefficient(
          equationindex, V_index,
          V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->add_to_coefficient(
          equationindex, phi_index,
          V_i * V_k * (-G_ik * sin(phi_ik) + B_ik * cos(phi_ik)));

      jacobianhandler->set_coefficient(
          equationindex, V_index_k,
          V_i * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->set_coefficient(
          equationindex, phi_index_k,
          V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
    }
  }

  void Powernode::evaluate_Q_derivative(
      int equationindex, Aux::Matrixhandler *jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    double B_i = get_B();
    double V_i = new_state[V_index];
    double phi_i = new_state[phi_index];

    jacobianhandler->set_coefficient(equationindex, V_index, -2 * B_i * V_i);
    jacobianhandler->set_coefficient(equationindex, phi_index, 0.0);

    for (auto &triple : attached_component_data) {
      double G_ik = std::get<0>(triple);
      double B_ik = std::get<1>(triple);
      Powernode *othernode = std::get<2>(triple);
      int V_index_k = othernode->get_start_state_index();
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler->add_to_coefficient(
          equationindex, V_index,
          V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler->add_to_coefficient(
          equationindex, phi_index,
          V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->set_coefficient(
          equationindex, V_index_k,
          V_i * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler->set_coefficient(
          equationindex, phi_index_k,
          V_i * V_k * (-G_ik * cos(phi_ik) - B_ik * sin(phi_ik)));
    }
  }

} // namespace Model::Networkproblem::Power
