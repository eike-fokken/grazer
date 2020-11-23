#include <Matrixhandler.hpp>
#include <PVnode.hpp>
#include <Transmissionline.hpp>
#include <iostream>

namespace Model::Networkproblem::Power {

  void PVnode::evaluate(Eigen::VectorXd &rootfunction, double, double new_time,
                        Eigen::VectorXd const &,
                        Eigen::VectorXd const &new_state) {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    rootfunction[V_index] = P(new_state) - boundaryvalue(new_time)[0];

    rootfunction[phi_index] = new_state[V_index] - boundaryvalue(new_time)[1];
  }

  void PVnode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                         double // last_time
                                         ,
                                         double // new_time
                                         ,
                                         Eigen::VectorXd const &,
                                         Eigen::VectorXd const &new_state) {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    evaluate_P_derivative(jacobianhandler, new_state);
    jacobianhandler->set_coefficient(phi_index, V_index, 1.0);
  }

  void PVnode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "type: PV, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
