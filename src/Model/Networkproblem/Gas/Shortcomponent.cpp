#include "Shortcomponent.hpp"
#include "Matrixhandler.hpp"
#include "Exception.hpp"
#include <iostream>
#include <fstream>

namespace Model::Networkproblem::Gas {

  void Shortcomponent::print_helper(std::filesystem::path const &output_directory, std::string const & type) {

    std::string prestring = "Gas_"+type+"_";
    std::string pressure_file_name = (get_id().insert(0, prestring))+"_p";
    std::string flow_file_name =(get_id().insert(0, prestring))+"_q";
    std::filesystem::path shortcomponent_output_pressure(output_directory /
                                                pressure_file_name);
    std::filesystem::path shortcomponent_output_flow(
        output_directory / flow_file_name);

    std::ofstream outputpressure(shortcomponent_output_pressure);
    std::ofstream outputflow(shortcomponent_output_flow);

    outputpressure.precision(9);
    outputflow.precision(9);

    outputpressure << "t-x,    0.0,    1.0\n";
    outputflow << "t-x,    0.0,    1.0\n";
    auto times = get_times();
    auto values = get_values();

    for (unsigned i = 0; i != times.size(); ++i) {
      {
        //write out pressures:
        auto var = values[i][0];
        outputpressure << times[i];
        outputpressure << ",    " << var.at(0.0);
        outputpressure << ",    " << var.at(1.0);
        outputpressure << std::endl;
      }
      {
        //write out flows:
        outputflow << times[i];
        auto var = values[i][1];
        outputflow << ",    " << var.at(0.0);
        outputflow << ",    " << var.at(1.0);
        outputflow << std::endl;
      }
    }
  }

  int Shortcomponent::get_number_of_states() const { return number_of_state_variables; }

  void Shortcomponent::save_values(double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
    std::map<double, double> pressure_map;
    std::map<double, double> flow_map;

    auto start_state= get_boundary_state(1,state);
    auto end_state = get_boundary_state(-1,state);

    pressure_map = {{0.0, start_state[0]},{1.0, end_state[0]}};
    flow_map = {{0.0, start_state[1]}, {1.0, end_state[1]}};

    std::vector<std::map<double, double>> value_vector({pressure_map, flow_map});
    Equationcomponent::push_to_values(time, value_vector);
  }

  void Shortcomponent::initial_values_helper(Eigen::Ref<Eigen::VectorXd> new_state,
                                             nlohmann::ordered_json initial_json, std::string const & type) {
    if (get_start_state_index() == -1 or get_after_state_index() == -1) {
        gthrow({"This function may only be called if set_indices  has been "
                "called beforehand!"});
      }

      // This tests whether the json is in the right format:
      try{
      if ((not initial_json["data"].is_array()) or (not initial_json["data"][0]["values"].is_array()) or initial_json["data"][0]["values"].size() != 2 or initial_json["data"].size()!=2) {
        std::cout << "The initial json for this " << type <<
                     " is given by:"
                  << "\n";
        std::cout << initial_json << std::endl;
        gthrow({"This is not a ", type, " initial condition!", "\n Maybe there are array brackets [] missing? "});
      }
      } catch (...) {
        std::cout << __FILE__ << ":"<< __LINE__ << " The exception was thrown here." << std::endl;
        throw;
      }
      auto start_p_index = get_boundary_state_index(1);
      auto start_q_index = start_p_index + 1;
      auto end_p_index = get_boundary_state_index(-1);
      auto end_q_index = end_p_index + 1;
      try {
      new_state[start_p_index] = initial_json["data"][0]["values"][0];
      new_state[start_q_index] = initial_json["data"][0]["values"][1];
      new_state[end_p_index] = initial_json["data"][1]["values"][0];
      new_state[end_q_index] = initial_json["data"][1]["values"][1];
      } catch(...){
        std::cout << __FILE__ << ":" << __LINE__
                  << ": failed to read in initial values in " << type << "!"
                  << std::endl;
        throw;
      }
  }

  Eigen::Vector2d Shortcomponent::get_boundary_p_qvol_bar(int direction, Eigen::Ref<Eigen::VectorXd const> const &state) const {
    return get_boundary_state(direction,state);
  }

  void Shortcomponent::dboundary_p_qvol_dstate(int direction, Aux::Matrixhandler * jacobianhandler, Eigen::RowVector2d function_derivative, int rootvalues_index, Eigen::Ref<Eigen::VectorXd const> const &) const {
    int p_index = get_boundary_state_index(direction);
    int q_index = p_index+1;

  
      jacobianhandler->set_coefficient(rootvalues_index,p_index, function_derivative[0]);
      jacobianhandler->set_coefficient(rootvalues_index,q_index, function_derivative[1]);
  
  }

}
