#include "Boundaryvalue.hpp"
#include "nlohmann/json.hpp"
#include <Exception.hpp>
#include <Matrixhandler.hpp>
#include <Powernode.hpp>
#include <Transmissionline.hpp>
#include <iostream>
#include <map>

namespace Model::Networkproblem::Power {

  Powernode::Powernode(std::string _id, nlohmann::ordered_json boundary_json,
                       double _G, double _B)
      : Node(_id), G(_G), B(_B) {
    set_boundary_condition(boundary_json);
  }

  void Powernode::set_boundary_condition(nlohmann::ordered_json boundary_json) {
    std::map<double, Eigen::Vector2d> boundary_map;
    for (auto &datapoint : boundary_json["data"]) {
      if (datapoint["values"].size() != 2) {
        gthrow(
            {"Wrong number of boundary values in node ", boundary_json["id"]});
      }
      Eigen::Vector2d value;
      try {
        value[0] = datapoint["values"][0];
        value[1] = datapoint["values"][1];
      } catch (...) {
        gthrow({"data in node with id ", boundary_json["id"],
                " couldn't be assignd in vector, not a double?"})
      }
      boundary_map.insert({datapoint["time"], value});
    }
    boundaryvalue = Boundaryvalue<Powernode, 2>(boundary_map);
  }

  void Powernode::set_initial_values(Eigen::VectorXd &new_state,
                                     nlohmann::ordered_json initial_json) {

    if (get_start_state_index() == -1) {
      gthrow({"This function may only be called if set_indices  has been "
              "called beforehand!"});
    }
    // this must change!
    if (initial_json["data"]["value"].size() != 4) {
      std::cout << "The initial json for this power node is given by:"
                << "\n";
      std::cout << initial_json << std::endl;
      gthrow({"This is not a power initial condition!"});
    }
    int index1 = get_start_state_index();
    int index2 = index1 + 1;
    new_state[index1] = initial_json["data"]["value"][2];
    new_state[index2] = initial_json["data"]["value"][3];
  }

  int Powernode::get_number_of_states() const { return 2; }

  double Powernode::get_G() const { return G; }

  double Powernode::get_B() const { return B; }

  void Powernode::print_to_files(std::filesystem::path &output_directory) {}

  void Powernode::save_values(double time, Eigen::VectorXd const &state) {
    std::vector<std::map<double, double>> value_vector;
    for (int i = get_start_state_index(); i != get_after_state_index(); ++i) {
      std::map<double, double> m({{0.0, state[i]}});
      value_vector.push_back(m);
    }
    Equationcomponent::push_to_values(time, value_vector);
  }

  double Powernode::P(double new_time, Eigen::VectorXd const &new_state) {
    int index1 = get_start_state_index();
    int index2 = index1 + 1;
    double G_i = get_G();

    double V_i = new_state[index1];
    double phi_i = new_state[index2];

    double P = -boundaryvalue(new_time)[0];
    P += G_i * V_i * V_i;
    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      auto othernode = dynamic_cast<Powernode *>(start_edge->get_ending_node());
      int index1_k = othernode->get_start_state_index();
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;
      P += V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik));
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      Powernode *othernode =
          dynamic_cast<Powernode *>(end_edge->get_starting_node());
      int index1_k = othernode->get_start_state_index();
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;
      P += V_i * V_k * (G_ik * cos(phi_ik) - B_ik * sin(phi_ik));
    }
    return P;
  }

  double Powernode::Q(double new_time, Eigen::VectorXd const &new_state) {

    int index1 = get_start_state_index();
    int index2 = index1 + 1;
    double B_i = get_B();
    double V_i = new_state[index1];
    double phi_i = new_state[index2];

    double Q = -boundaryvalue(new_time)[1];
    Q -= B_i * V_i * V_i;
    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      auto othernode = dynamic_cast<Powernode *>(start_edge->get_ending_node());
      int index1_k = othernode->get_start_state_index();
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;
      Q += V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik));
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      Powernode *othernode =
          dynamic_cast<Powernode *>(end_edge->get_starting_node());
      int index1_k = othernode->get_start_state_index();
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;
      Q += V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik));
    }
    return Q;
  }

  void Powernode::evaluate_P_derivative(Aux::Matrixhandler *jacobianhandler,
                                        Eigen::VectorXd const &new_state) {
    int index1 = get_start_state_index();
    int index2 = index1 + 1;
    double G_i = get_G();
    double V_i = new_state[index1];
    double phi_i = new_state[index2];

    jacobianhandler->set_coefficient(index1, index1, 2 * G_i * V_i);
    jacobianhandler->set_coefficient(index1, index2, 0.0);

    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      auto othernode = dynamic_cast<Powernode *>(start_edge->get_ending_node());
      int index1_k = othernode->get_start_state_index();
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler->add_to_coefficient(
          index1, index1, V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->add_to_coefficient(
          index1, index2,
          V_i * V_k * (-G_ik * sin(phi_ik) + B_ik * cos(phi_ik)));
      jacobianhandler->set_coefficient(
          index1, index1_k, V_i * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->set_coefficient(
          index1, index2_k,
          V_i * V_k * (G_ik * sin(phi_ik) + B_ik * cos(phi_ik)));
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      Powernode *othernode =
          dynamic_cast<Powernode *>(end_edge->get_starting_node());
      int index1_k = othernode->get_start_state_index();
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;
      jacobianhandler->add_to_coefficient(
          index1, index1, V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->add_to_coefficient(
          index1, index2,
          V_i * V_k * (-G_ik * sin(phi_ik) + B_ik * cos(phi_ik)));
      jacobianhandler->set_coefficient(
          index1, index1_k, V_i * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->set_coefficient(
          index1, index2_k,
          V_i * V_k * (G_ik * sin(phi_ik) + B_ik * cos(phi_ik)));
    }
  }

  void Powernode::evaluate_Q_derivative(Aux::Matrixhandler *jacobianhandler,
                                        Eigen::VectorXd const &new_state) {
    int index1 = get_start_state_index();
    int index2 = index1 + 1;
    double B_i = get_B();
    double V_i = new_state[index1];
    double phi_i = new_state[index2];

    jacobianhandler->set_coefficient(index2, index1, -2 * B_i * V_i);
    jacobianhandler->set_coefficient(index2, index2, 0.0);

    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      auto othernode = dynamic_cast<Powernode *>(start_edge->get_ending_node());
      int index1_k = othernode->get_start_state_index();
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler->add_to_coefficient(
          index2, index1, V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler->add_to_coefficient(
          index2, index2,
          V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->set_coefficient(
          index2, index1_k, V_i * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler->set_coefficient(
          index2, index2_k,
          V_i * V_k * (-G_ik * cos(phi_ik) - B_ik * sin(phi_ik)));
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      Powernode *othernode =
          dynamic_cast<Powernode *>(end_edge->get_starting_node());
      int index1_k = othernode->get_start_state_index();
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler->add_to_coefficient(
          index2, index1, V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler->add_to_coefficient(
          index2, index2,
          V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->set_coefficient(
          index2, index1_k, V_i * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
      jacobianhandler->set_coefficient(
          index2, index2_k,
          V_i * V_k * (-G_ik * cos(phi_ik) - B_ik * sin(phi_ik)));
    }
  }

} // namespace Model::Networkproblem::Power
