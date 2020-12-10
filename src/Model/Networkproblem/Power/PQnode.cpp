#include <Matrixhandler.hpp>
#include <PQnode.hpp>
#include <Transmissionline.hpp>
#include <cmath>
#include <iostream>
#include <math.h>

namespace Model::Networkproblem::Power {

  void PQnode::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
                        Eigen::Ref<Eigen::VectorXd const> const &,
                        Eigen::Ref<Eigen::VectorXd const> const &new_state) const{

    int P_index = get_start_state_index();
    int Q_index = P_index +1;
    int V_index = P_index +2;
    int phi_index = P_index + 3;

    rootvalues[P_index] =  P(new_state)-new_state[P_index];
    rootvalues[Q_index] =  Q(new_state)-new_state[Q_index];
    rootvalues[V_index] = new_state[P_index] - boundaryvalue(new_time)[0];
    rootvalues[phi_index] = new_state[Q_index] - boundaryvalue(new_time)[1];
  }

  void PQnode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                         double // last_time
                                         ,
                                         double // new_time
                                         ,
                                         Eigen::Ref<Eigen::VectorXd const> const &,
                                         Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

    int P_index = get_start_state_index();
    int Q_index = P_index +1;
    int V_index = P_index +2;
    int phi_index = P_index + 3;

    jacobianhandler->set_coefficient(P_index, P_index, -1.0);
    evaluate_P_derivative(P_index, jacobianhandler, new_state);

    jacobianhandler->set_coefficient(Q_index, Q_index, -1.0);
    evaluate_Q_derivative(Q_index, jacobianhandler, new_state);

    jacobianhandler->set_coefficient(V_index, P_index, 1.0);
    jacobianhandler->set_coefficient(phi_index, Q_index, 1.0);
  

  }

  void PQnode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "type: PQ, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
