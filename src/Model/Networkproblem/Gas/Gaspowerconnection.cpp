#include <Gaspowerconnection.hpp>
#include <Exception.hpp>
#include <iostream>

namespace Model::Networkproblem::Gas {

  Gaspowerconnection::Gaspowerconnection(std::string _id,
                                         Network::Node *start_node,
                                         Network::Node *end_node,
                                         nlohmann::ordered_json topology_json)
      : Gasedge(_id, start_node, end_node),
        efficiency_gas2power(std::stod(topology_json["gas2power_efficiency"].get<std::string>())),
        efficiency_power2gas(
            std::stod(topology_json["power2gas_efficiency"].get<std::string>())) {}

  void Gaspowerconnection::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                                    double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                                    Eigen::Ref<Eigen::VectorXd const> const &new_state) const {}
  void Gaspowerconnection::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
                                                     Eigen::Ref<Eigen::VectorXd const> const &, Eigen::Ref<Eigen::VectorXd const> const &new_state) const {



  }


  int Gaspowerconnection::get_number_of_states() const { return 2;}

  void
  Gaspowerconnection::print_to_files(std::filesystem::path const &output_directory) {}

  void Gaspowerconnection::save_values(double time, Eigen::Ref<Eigen::VectorXd const> const &state) {}

  void Gaspowerconnection::set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                                              nlohmann::ordered_json initial_json) {
    if (get_start_state_index() == -1 or get_after_state_index() == -1) {
      gthrow({"This function may only be called if set_indices  has been "
              "called beforehand!"});
    }
    // This tests whether the json is in the right format:
    try {
      if ( (!initial_json["data"]["value"].is_array()) or
          initial_json["data"]["value"].size() != 2) {
        std::cout << "The initial json for this gaspowerconnection is given by:"
                  << "\n";
        std::cout << initial_json << std::endl;
        gthrow({"This is not a gaspowerconnection initial condition!"});
      }
    } catch (...) {
      std::cout << __FILE__ << ":" << __LINE__
                << " The exception was thrown here." << std::endl;
      throw;
    }
    auto start_p_index = get_boundary_state_index(1);
    auto start_q_index = start_p_index + 1;
    try {
      new_state[start_p_index] = initial_json["data"]["value"][0];
      new_state[start_q_index] = initial_json["data"]["value"][1];
    } catch (...) {
      std::cout << __FILE__ << ":" << __LINE__
                << ": failed to read in initial values in gaspowerconnection!"
                << std::endl;
      throw;
    }
  }


  Eigen::Vector2d Gaspowerconnection::get_boundary_p_qvol(int direction, Eigen::Ref<Eigen::VectorXd const> const &state) const {
    gthrow({" You must implement me!"})
  }


  void Gaspowerconnection::dboundary_p_qvol_dstate(int direction, Aux::Matrixhandler * jacobianhandler, Eigen::RowVector2d function_derivative, int rootvalues_index, Eigen::Ref<Eigen::VectorXd const> const &state) const {}


}
