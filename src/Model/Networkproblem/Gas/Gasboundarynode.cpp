#include <Exception.hpp>
#include <Gasboundarynode.hpp>
#include <iostream>
#include <Eigen/Sparse>

namespace Model::Networkproblem::Gas {

  Gasboundarynode::Gasboundarynode(std::string _id, nlohmann::ordered_json boundary_json,
                 nlohmann::ordered_json topology_json)
    : Node(_id),boundarytype(boundary_json["type"].get<std::string>()) {
    set_boundary_condition(boundary_json);
  }

  void Gasboundarynode::set_boundary_condition(nlohmann::ordered_json boundary_json) {
    std::map<double, Eigen::Matrix<double,1,1>> boundary_map;
    for (auto &datapoint : boundary_json["data"]) {
      if (datapoint["values"].size() != 1) {
        gthrow(
            {"Wrong number of boundary values in node ", boundary_json["id"]});
      }
      Eigen::Matrix<double, 1, 1> value;
      try {
        value[0] = datapoint["values"][0];
      } catch (...) {
        gthrow({"data in node with id ", boundary_json["id"],
                " couldn't be assignd in vector, not a double?"})
      }
      boundary_map.insert({datapoint["time"], value});
    }
    boundaryvalue = Boundaryvalue<Gasboundarynode, 1>(boundary_map);
  }

  void Gasboundarynode::evaluate(Eigen::VectorXd &rootfunction, double last_time,
                double new_time, Eigen::VectorXd const &last_state,
                Eigen::VectorXd const &new_state) {
    // std::cout << "you have to implement me!!!" << __FILE__ << ":" << __LINE__ << ", function: " << __FUNCTION__ << std::endl;
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
