#include <Exception.hpp>
#include <Flowboundarynode.hpp>
#include <iostream>
#include <Eigen/Sparse>

namespace Model::Networkproblem::Gas {

  Flowboundarynode::Flowboundarynode(std::string _id, nlohmann::ordered_json boundary_json,
                 nlohmann::ordered_json topology_json)
    : Gasnode(_id) {
    boundaryvalue.set_boundary_condition(boundary_json);
  }

  void Flowboundarynode::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double ,
                double new_time, Eigen::VectorXd const &,
                Eigen::VectorXd const &new_state) const{

    if(directed_attached_gas_edges.empty()){ return; }

        evaluate_flow_node_balance(rootvalues,new_state, boundaryvalue(new_time)[0]);
      }

  void Flowboundarynode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                 double , double ,
                                 Eigen::VectorXd const &,
                                 Eigen::VectorXd const &new_state) const{
    if(directed_attached_gas_edges.empty()){ return; }
    evaluate_flow_node_derivative(jacobianhandler,new_state);
  }

  void Flowboundarynode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "NO FURTHER DISPLAY PARAMETERS IMPLEMENTED IN" << __FILE__ << ":" << __LINE__ << std::endl;
    // here show information of the gas node!
  }
}
