#include <Matrixhandler.hpp>
#include <Vphinode.hpp>
#include <iostream>

namespace Model::Networkproblem::Power {

  void Vphinode::evaluate(Eigen::VectorXd &rootfunction, double // last_time
                          ,
                          double new_time,
                          Eigen::VectorXd const & // last_state
                          ,
                          Eigen::VectorXd const &new_state) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    rootfunction[V_index] = new_state[V_index] - boundaryvalue(new_time)[0];

    rootfunction[phi_index] = new_state[phi_index] - boundaryvalue(new_time)[1];
  }

  void Vphinode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                           double // last_time
                                           ,
                                           double // new_time
                                           ,
                                           Eigen::VectorXd const &,
                                           Eigen::VectorXd const & // new_state
  ) const {
    int V_index = get_start_state_index();
    int phi_index = V_index + 1;
    jacobianhandler->set_coefficient(V_index, V_index, 1.0);
    jacobianhandler->set_coefficient(phi_index, phi_index, 1.0);
  }

  void Vphinode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "type: Vphi, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
