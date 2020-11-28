#include <Innode.hpp>
#include <iostream>

namespace Model::Networkproblem::Gas {


  void Innode::evaluate(Eigen::VectorXd & rootfunction, double ,
              double , Eigen::VectorXd const &,
                        Eigen::VectorXd const &new_state) const {
    if(directed_attached_gas_edges.empty()){
      std::cout << "Node " <<get_id() << " has no attached gas edges!" << std::endl;
      return;
    }
      evaluate_flow_node_balance(rootfunction,new_state, 0.0);

  }
  void Innode::evaluate_state_derivative(Aux::Matrixhandler * jacobianhandler, double, double,
                                         Eigen::VectorXd const &, Eigen::VectorXd const &new_state) const {
    if(directed_attached_gas_edges.empty()){
      std::cout << "Node " <<get_id() << " has no attached gas edges!" << std::endl;
      return;
    }
      evaluate_flow_node_derivative(jacobianhandler,new_state);


  }

  void Innode::display() const {

  }

}
