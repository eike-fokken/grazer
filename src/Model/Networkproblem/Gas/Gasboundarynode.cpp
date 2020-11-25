#include <Exception.hpp>
#include <Gasboundarynode.hpp>
#include <iostream>
#include <Eigen/Sparse>

namespace Model::Networkproblem::Gas {

  Gasboundarynode::Gasboundarynode(std::string _id, nlohmann::ordered_json boundary_json,
                 nlohmann::ordered_json topology_json)
    : Node(_id) {
    boundaryvalue.set_boundary_condition(boundary_json);
        if(boundary_json["type"].get<std::string>() == "pressure"){
          btype= pressure;
        } else if(boundary_json["type"].get<std::string>() == "flow"){
          btype= flow;
        } else if (boundary_json["type"].get<std::string>() == "negflow") {
          btype = negflow;
        } else {
          gthrow({"Gas node with id :", _id, " has unknown boundary type: \"",
                  boundary_json["type"].get<std::string>(),
                  "\", should be \"pressure\", \"flow\" or \"negflow\".\n"});
        }
  }

  void Gasboundarynode::evaluate(Eigen::VectorXd &rootfunction, double last_time,
                double new_time, Eigen::VectorXd const &last_state,
                Eigen::VectorXd const &new_state) {
  }
  void Gasboundarynode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                 double last_time, double new_time,
                                 Eigen::VectorXd const &,
                                 Eigen::VectorXd const &new_state) {
    // std::cout << "you have to implement me!!!" << __FILE__ << ":" << __LINE__
    //           << ", function: " << __FUNCTION__ << std::endl;
  }

  int Gasboundarynode::get_number_of_states() const {
    return 0;
  }

  void Gasboundarynode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "NO FURTHER DISPLAY PARAMETERS IMPLEMENTED IN" << __FILE__ << ":" << __LINE__ << std::endl;
    // here show information of the gas node!
  }
}
