#include <Exception.hpp>
#include <Pressureboundarynode.hpp>
#include <iostream>
#include <Eigen/Sparse>

namespace Model::Networkproblem::Gas {

  Pressureboundarynode::Pressureboundarynode(std::string _id, nlohmann::ordered_json boundary_json,
                 nlohmann::ordered_json topology_json)
    : Gasnode(_id) {
    boundaryvalue.set_boundary_condition(boundary_json);
  }

  void Pressureboundarynode::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double ,
                double new_time, Eigen::VectorXd const &,
                Eigen::VectorXd const &new_state) const{

    if(directed_attached_gas_edges.empty()){ return; }
         evaluate_pressure_node_balance(rootvalues,new_state, boundaryvalue(new_time)[0]);
  }

  void Pressureboundarynode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                 double , double ,
                                 Eigen::VectorXd const &,
                                 Eigen::VectorXd const &new_state) const{
    if(directed_attached_gas_edges.empty()){ return; }
      evaluate_pressure_node_derivative(jacobianhandler,new_state);
  }

  void Pressureboundarynode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "NO FURTHER DISPLAY PARAMETERS IMPLEMENTED IN" << __FILE__ << ":" << __LINE__ << std::endl;
    // here show information of the gas node!
  }
}
