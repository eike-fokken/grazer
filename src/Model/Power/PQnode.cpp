#include "Transmissionline.hpp"
#include <PQnode.hpp>
#include <cmath>
#include <math.h>

namespace Model::Networkproblem::Power {

  void PQnode::evaluate(Eigen::VectorXd &rootfunction, double, double new_time,
                        Eigen::VectorXd const &,
                        Eigen::VectorXd const &new_state) {
    int index1 = static_cast<int>(get_start_state_index());
    int index2 = index1 + 1;
    double G_i = get_G();
    double B_i = get_B();
    double V_i = new_state[index1];
    double phi_i = new_state[index2];

    double P = -boundaryvalue(new_time)[0];
    double Q = -boundaryvalue(new_time)[1];
    P += G_i * V_i * V_i;
    Q -= B_i * V_i * V_i;
    for (auto &start_edge : get_starting_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(start_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      auto othernode = dynamic_cast<Powernode *>(start_edge->get_ending_node());
      int index1_k = static_cast<int>(othernode->get_start_state_index());
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;
      P += V_i * V_k * (G_ik * cos(phi_ik) + B_ik * sin(phi_ik));
      Q += V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik));
    }

    for (auto &end_edge : get_ending_edges()) {
      auto line = dynamic_cast<Transmissionline const *>(end_edge);
      double G_ik = line->get_G();
      double B_ik = line->get_B();
      Powernode *othernode =
          dynamic_cast<Powernode *>(end_edge->get_starting_node());
      int index1_k = static_cast<int>(othernode->get_start_state_index());
      int index2_k = index1_k + 1;
      double V_k = new_state[index1_k];
      double phi_k = new_state[index2_k];
      double phi_ik = phi_i - phi_k;
      P += V_i * V_k * (G_ik * cos(phi_ik) - B_ik * sin(phi_ik));
      Q += V_i * V_k * (G_ik * sin(phi_ik) - B_ik * cos(phi_ik));
    }
    rootfunction[index1] = P;
    rootfunction[index2] = Q;
  }

} // namespace Model::Networkproblem::Power
