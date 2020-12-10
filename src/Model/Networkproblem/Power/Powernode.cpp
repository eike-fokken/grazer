#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Vphinode.hpp"
#include <Boundaryvalue.hpp>
#include <Exception.hpp>
#include <Matrixhandler.hpp>
#include <Powernode.hpp>
#include <Transmissionline.hpp>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>

namespace Model::Networkproblem::Power {

  Powernode::Powernode(std::string _id, nlohmann::ordered_json boundary_json,
                       double _G, double _B)
      : Node(_id), G(_G), B(_B) {
    boundaryvalue.set_boundary_condition(boundary_json);
  }


  void Powernode::set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                                     nlohmann::ordered_json initial_json) {

    if (get_start_state_index() == -1) {
      gthrow({"This function may only be called if set_indices  has been "
              "called beforehand!"});
    }

    if (initial_json["data"]["value"].size() != 4) {
      std::cout << "The initial json for this power node is given by:"
                << "\n";
      std::cout << initial_json << std::endl;
      gthrow({"This is not a power initial condition!"});
    }
    int P_index = get_start_state_index();
    int Q_index = P_index +1;
    int V_index = P_index +2;
    int phi_index = P_index + 3;
    new_state[P_index] = initial_json["data"]["value"][0];
    new_state[Q_index] = initial_json["data"]["value"][1];
    new_state[V_index] = initial_json["data"]["value"][2];
    new_state[phi_index] = initial_json["data"]["value"][3];
  }

  int Powernode::get_number_of_states() const { return 4; }

  // int Powernode::get_number_of_printout_states() const { return 4;}

  double Powernode::get_G() const { return G; }

  double Powernode::get_B() const { return B; }

  void
  Powernode::print_to_files(std::filesystem::path const &output_directory) {
    std::filesystem::path node_output_directory(output_directory /
                                                (get_id().insert(0, "Power_")));


    std::ofstream output(node_output_directory);

    output << "time,    P,    Q,    V,    phi\n";
    auto values = get_values();
    output.precision(9);
    auto times = get_times();

    for (unsigned long i = 0; i != times.size(); ++i) {
      output << times[i];
      for (auto const &var : values[i]) {
        output << ",    " << var.at(0.0);
      }
      output << std::endl;
    }
    output << std::endl;
  }

  void Powernode::save_values(double time, Eigen::Ref<Eigen::VectorXd const> const &state) {

    std::map<double, double> Pmap;
    std::map<double, double> Qmap;
    std::map<double, double> Vmap;
    std::map<double, double> phimap;

    int P_index = get_start_state_index();
    int Q_index = P_index +1;
    int V_index = P_index +2;
    int phi_index = P_index + 3;

    std::vector<std::map<double, double>> value_vector;
    Pmap = {{0.0, state[P_index]}};
    Qmap = {{0.0, state[Q_index]}};
    Vmap = {{0.0, state[V_index]}};
    phimap = {{0.0, state[phi_index]}};

    value_vector = {Pmap, Qmap, Vmap, phimap};
    Equationcomponent::push_to_values(time, value_vector);
  }

  double Powernode::P(Eigen::Ref<Eigen::VectorXd const> const &new_state) const{
    int V_index = get_start_state_index()+2;
    int phi_index = V_index + 1;
    double G_i = get_G();

    double V_i = new_state[V_index];
    double phi_i = new_state[phi_index];

    double P = 0.0; // -boundaryvalue(new_time)[0];
    P += G_i * V_i * V_i;
    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      if (!line) {continue;}
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      auto othernode = dynamic_cast<Powernode *>(start_edge->get_ending_node());
      int V_index_k = othernode->get_start_state_index()+2;
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;
      P += V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik));
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      if (!line) {// std::cout << "no transmissionline!" <<std::endl;
        continue;
      }
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      Powernode *othernode =
          dynamic_cast<Powernode *>(end_edge->get_starting_node());
      int V_index_k = othernode->get_start_state_index()+2;
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;
      P += V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik));
    }
    return P;
  }

  double Powernode::Q(Eigen::Ref<Eigen::VectorXd const> const &new_state) const{

    int V_index = get_start_state_index()+2;
    int phi_index = V_index + 1;
    double B_i = get_B();
    double V_i = new_state[V_index];
    double phi_i = new_state[phi_index];

    double Q = 0.0; 
    Q -= B_i * V_i * V_i;
    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      if (!line) {// std::cout << "no transmissionline!" <<std::endl;
        continue;
      }
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      auto othernode = dynamic_cast<Powernode *>(start_edge->get_ending_node());
      int V_index_k = othernode->get_start_state_index()+2;
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;
      Q += V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik));
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      if (!line) {// std::cout << "no transmissionline!" <<std::endl;
        continue;
      }
        double G_ik = line->get_G();
        double B_ik = line->get_B();
        Powernode *othernode =
            dynamic_cast<Powernode *>(end_edge->get_starting_node());
        int V_index_k = othernode->get_start_state_index()+2;
        int phi_index_k = V_index_k + 1;
        double V_k = new_state[V_index_k];
        double phi_k = new_state[phi_index_k];
        double phi_ik = phi_i - phi_k;
        Q += V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik));
      }
    return Q;
  }

  void Powernode::evaluate_P_derivative(int equationindex, Aux::Matrixhandler *jacobianhandler,
                                        Eigen::Ref<Eigen::VectorXd const> const &new_state) const{
    int V_index = get_start_state_index()+2;
    int phi_index = V_index + 1;
    double G_i = get_G();
    double V_i = new_state[V_index];
    double phi_i = new_state[phi_index];

    jacobianhandler->set_coefficient(equationindex, V_index, 2 * G_i * V_i);
    jacobianhandler->set_coefficient(equationindex, phi_index, 0.0);

    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      if(!line) {continue;}
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      auto othernode = dynamic_cast<Powernode *>(start_edge->get_ending_node());
      int V_index_k = othernode->get_start_state_index()+2;
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler->add_to_coefficient(
          equationindex, V_index, V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->add_to_coefficient(
          equationindex, phi_index,
          V_i * V_k * (-G_ik * sin(phi_ik) + B_ik * cos(phi_ik)));

      jacobianhandler->set_coefficient(
          equationindex, V_index_k, V_i * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik)));
      jacobianhandler->set_coefficient(
          equationindex, phi_index_k,
          V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      if (!line) {// std::cout << "no transmissionline!" <<std::endl;
        continue;
      }
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      Powernode *othernode =
          dynamic_cast<Powernode *>(end_edge->get_starting_node());
      int V_index_k = othernode->get_start_state_index()+2;
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

  void Powernode::evaluate_Q_derivative(int equationindex, Aux::Matrixhandler *jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    int V_index = get_start_state_index()+2;
    int phi_index = V_index + 1;
    double B_i = get_B();
    double V_i = new_state[V_index];
    double phi_i = new_state[phi_index];

    jacobianhandler->set_coefficient(equationindex, V_index,-2 * B_i * V_i);
    jacobianhandler->set_coefficient(equationindex, phi_index, 0.0);

    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      if (!line) {// std::cout << "no transmissionline!" <<std::endl;
        continue;
      }
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      auto othernode = dynamic_cast<Powernode *>(start_edge->get_ending_node());
      int V_index_k = othernode->get_start_state_index()+2;
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler->add_to_coefficient(
          equationindex, V_index, V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
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

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      if (!line) {// std::cout << "no transmissionline!" <<std::endl;
        continue;
      }
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      Powernode *othernode =
          dynamic_cast<Powernode *>(end_edge->get_starting_node());
      int V_index_k = othernode->get_start_state_index()+2;
      int phi_index_k = V_index_k + 1;
      double V_k = new_state[V_index_k];
      double phi_k = new_state[phi_index_k];
      double phi_ik = phi_i - phi_k;

      jacobianhandler->add_to_coefficient(
          equationindex, V_index, V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik)));
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
