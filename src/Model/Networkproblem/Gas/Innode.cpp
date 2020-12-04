#include <Innode.hpp>
#include <iostream>

namespace Model::Networkproblem::Gas {


  void Innode::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double ,
              double , Eigen::VectorXd const &,
                        Eigen::VectorXd const &new_state) const {
      evaluate_flow_node_balance(rootvalues,new_state, 0.0);
  }
  void Innode::evaluate_state_derivative(Aux::Matrixhandler * jacobianhandler, double, double,
                                         Eigen::VectorXd const &, Eigen::VectorXd const &new_state) const {
      evaluate_flow_node_derivative(jacobianhandler,new_state);


  }

  void Innode::display() const {

  }

}
