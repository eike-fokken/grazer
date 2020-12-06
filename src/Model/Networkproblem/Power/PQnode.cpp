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
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    rootvalues[V_index] = P(new_state) - boundaryvalue(new_time)[0];
    rootvalues[phi_index] = Q(new_state) - boundaryvalue(new_time)[1];
  }

  void PQnode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                         double // last_time
                                         ,
                                         double // new_time
                                         ,
                                         Eigen::Ref<Eigen::VectorXd const> const &,
                                         Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    evaluate_P_derivative(jacobianhandler, new_state);
    evaluate_Q_derivative(jacobianhandler, new_state);
  }

  void PQnode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "type: PQ, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
