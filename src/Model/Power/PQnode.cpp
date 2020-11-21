#include <Matrixhandler.hpp>
#include <PQnode.hpp>
#include <Transmissionline.hpp>
#include <cmath>
#include <iostream>
#include <math.h>

namespace Model::Networkproblem::Power {

  void PQnode::evaluate(Eigen::VectorXd &rootfunction, double, double new_time,
                        Eigen::VectorXd const &,
                        Eigen::VectorXd const &new_state) {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    rootfunction[V_index] = P(new_time, new_state);
    rootfunction[phi_index] = Q(new_time, new_state);
  }

  void PQnode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                         double // last_time
                                         ,
                                         double // new_time
                                         ,
                                         Eigen::VectorXd const &,
                                         Eigen::VectorXd const &new_state) {
    evaluate_P_derivative(jacobianhandler, new_state);
    evaluate_Q_derivative(jacobianhandler, new_state);
  }

  void PQnode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "type: PQ, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
