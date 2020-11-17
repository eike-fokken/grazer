#include <Matrixhandler.hpp>
#include <Powernode.hpp>
#include <Transmissionline.hpp>

namespace Model::Networkproblem::Power {

  int Powernode::get_number_of_states() const { return 2; }

  double Powernode::get_G() const { return G; }

  double Powernode::get_B() const { return B; }

  void Powernode::push_values(double time, Eigen::VectorXd const &state) {
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
