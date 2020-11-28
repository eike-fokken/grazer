#include <Exception.hpp>
#include <Gasboundarynode.hpp>
#include <iostream>
#include <Eigen/Sparse>

namespace Model::Networkproblem::Gas {

  Gasboundarynode::Gasboundarynode(std::string _id, nlohmann::ordered_json boundary_json,
                 nlohmann::ordered_json topology_json)
    : Gasnode(_id) {
    boundaryvalue.set_boundary_condition(boundary_json);
        if(boundary_json["type"].get<std::string>() == "pressure"){
          btype= pressure;
        } else if(boundary_json["type"].get<std::string>() == "flow"){
          btype= flow;
        } else {
          gthrow({"Gas node with id :", _id, " has unknown boundary type: \"",
                  boundary_json["type"].get<std::string>(),
                  "\", should be \"pressure\" or \"flow\".\n"});
        }
  }

  void Gasboundarynode::evaluate(Eigen::VectorXd &rootfunction, double ,
                double new_time, Eigen::VectorXd const &,
                Eigen::VectorXd const &new_state) const{

    if(directed_attached_gas_edges.empty()){
      std::cout << "Node " <<get_id() << " has no attached gas edges!" << std::endl;
      return;
    }

    if(btype == flow){
    evaluate_flow_node_balance(rootfunction,new_state, boundaryvalue(new_time)[0]);
    } else if(btype == pressure){
      evaluate_pressure_node_balance(rootfunction,new_state, boundaryvalue(new_time)[0]);
    } else {
      std::cout << __FILE__ << ":" <<__LINE__ << ": No boundary type other than pressure and flow are supported!" << std::endl;
    }

  }

  void Gasboundarynode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                 double , double ,
                                 Eigen::VectorXd const &,
                                 Eigen::VectorXd const &new_state) const{
    if(directed_attached_gas_edges.empty()){
      std::cout << "Node " <<get_id() << " has no attached gas edges!" << std::endl;
      return;
    }

    if(btype == flow){
      evaluate_flow_node_derivative(jacobianhandler,new_state);
    } else if(btype == pressure){
      evaluate_pressure_node_derivative(jacobianhandler,new_state);
    } else {
      std::cout << __FILE__ << ":" <<__LINE__ << ": No boundary type other than pressure and flow are supported!" << std::endl;
    }
  }

  void Gasboundarynode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "NO FURTHER DISPLAY PARAMETERS IMPLEMENTED IN" << __FILE__ << ":" << __LINE__ << std::endl;
    // here show information of the gas node!
  }
}
