#include <Shortpipe.hpp>
#include <Matrixhandler.hpp>
#include <Exception.hpp>
#include <iostream>
#include <fstream>

namespace Model::Networkproblem::Gas {

  void Shortpipe::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double ,
                double , Eigen::VectorXd const &,
                Eigen::VectorXd const &new_state) const {
    rootvalues.segment<2>(get_equation_start_index()) =
      get_boundary_state(1,new_state) - get_boundary_state(-1,new_state);
  }

  void Shortpipe::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double , double ,
      Eigen::VectorXd const &, Eigen::VectorXd const &) const {

    auto start_p_index=get_boundary_state_index(1);
    auto start_q_index = start_p_index+1;
    auto end_p_index = get_boundary_state_index(-1);
    auto end_q_index = end_p_index + 1;

    auto start_equation_index = get_equation_start_index();
    auto end_equation_index = start_equation_index+1;

    jacobianhandler->set_coefficient(start_equation_index, start_p_index, 1.0);
    jacobianhandler->set_coefficient(start_equation_index, end_p_index, -1.0);
    jacobianhandler->set_coefficient(end_equation_index, start_q_index, 1.0);
    jacobianhandler->set_coefficient(end_equation_index, end_q_index, -1.0);
  }

  void Shortpipe::print_to_files(std::filesystem::path const &output_directory) {
    std::filesystem::path shortpipe_output_pressure(output_directory /
                                                (get_id().insert(0, "Gas_Shortpipe_p_")));
    std::filesystem::path shortpipe_output_flow(
        output_directory / (get_id().insert(0, "Gas_Shortpipe_q_")));

    std::ofstream outputpressure(shortpipe_output_pressure);
    std::ofstream outputflow(shortpipe_output_flow);

    
    outputpressure << "t-x,\t 0.0,\t 1.0\n";
    outputflow << "t-x,\t 0.0,\t 1.0\n";
    auto times = get_times();
    auto values = get_values();

    //write out pressures:
    for (unsigned i = 0; i != times.size(); ++i) {
      {
        auto var = values[i][0];
        outputpressure << times[i];
        outputpressure << ",\t " << var.at(0.0);
        outputpressure << ",\t " << var.at(1.0);
        outputpressure << std::endl;
      }
      {
        outputflow << times[i];
        auto var = values[i][1];
        outputflow << ",\t " << var.at(0.0);
        outputflow << ",\t " << var.at(1.0);
        outputflow << std::endl;
      }
    }
  }

  int Shortpipe::get_number_of_states() const { return 4; }

  void Shortpipe::save_values(double time, Eigen::VectorXd const &state) {
    std::map<double, double> pressure_map;
    std::map<double, double> flow_map;

    auto start_state= get_boundary_state(1,state);
    auto end_state = get_boundary_state(-1,state);

    pressure_map = {{0.0, start_state[0]},{1.0, end_state[0]}};
    flow_map = {{0.0, start_state[1]}, {1.0, end_state[1]}};

    std::vector<std::map<double, double>> value_vector({pressure_map, flow_map});
    Equationcomponent::push_to_values(time, value_vector);
  }

  void Shortpipe::set_initial_values(Eigen::VectorXd &new_state,
                                     nlohmann::ordered_json initial_json) {
    if (get_start_state_index() == -1 or get_after_state_index() == -1) {
        gthrow({"This function may only be called if set_indices  has been "
                "called beforehand!"});
      }

      // This tests whether the json is in the right format:
      try{
      if ((!initial_json["data"].is_array()) or (!initial_json["data"][0]["value"].is_array()) or initial_json["data"][0]["value"].size() != 2 or initial_json["data"].size()!=2) {
        std::cout << "The initial json for this shortpipe is given by:"
                  << "\n";
        std::cout << initial_json << std::endl;
        gthrow({"This is not a shortpipe initial condition!", "\n Maybe there are array brackest [] missing? "});
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
      new_state[start_p_index] = initial_json["data"][0]["value"][0];
      new_state[start_q_index] = initial_json["data"][0]["value"][1];
      new_state[end_p_index] = initial_json["data"][1]["value"][0];
      new_state[end_q_index] = initial_json["data"][1]["value"][1];
      } catch(...){
        std::cout << __FILE__ <<":" << __LINE__ << ": failed to read in initial values in shortpipe!" << std::endl;
        throw;
      }
  }

  Eigen::Vector2d Shortpipe::get_boundary_p_qvol(int direction, Eigen::VectorXd const &state) const {
    return get_boundary_state(direction,state);
  }

  void Shortpipe::dboundary_p_qvol_dstate(int direction, Aux::Matrixhandler * jacobianhandler, Eigen::RowVector2d function_derivative, int rootvalues_index, Eigen::VectorXd const &) const {
    int p_index = get_boundary_state_index(direction);
    int q_index = p_index+1;
    jacobianhandler->set_coefficient(rootvalues_index,p_index, function_derivative[0]);
    jacobianhandler->set_coefficient(rootvalues_index,q_index, function_derivative[1]);
    }

}
