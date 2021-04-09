#include "Shortcomponent.hpp"
#include "Exception.hpp"
#include "Initialvalue.hpp"
#include "Matrixhandler.hpp"
#include <fstream>
#include <iostream>

namespace Model::Networkproblem::Gas {

  void Shortcomponent::print_helper(
      std::filesystem::path const &output_directory, std::string const &type) {

    std::string prestring = "Gas_" + type + "_";
    std::string pressure_file_name = get_id_copy() + "_p";
    std::string flow_file_name = get_id_copy() + "_q";

    std::filesystem::path outputfile_pressure
        = output_directory / std::filesystem::path(type)
          / std::filesystem::path(pressure_file_name);
    std::filesystem::path outputfile_flow
        = output_directory / std::filesystem::path(type)
          / std::filesystem::path(flow_file_name);

    std::ofstream outputpressure(outputfile_pressure);
    std::ofstream outputflow(outputfile_flow);

    outputpressure.precision(9);
    outputflow.precision(9);

    outputpressure << "t-x,    0.0,    1.0\n";
    outputflow << "t-x,    0.0,    1.0\n";
    auto times = get_times();
    auto values = get_values();

    for (unsigned i = 0; i != times.size(); ++i) {
      {
        // write out pressures:
        auto var = values[i][0];
        outputpressure << times[i];
        outputpressure << ",    " << var.at(0.0);
        outputpressure << ",    " << var.at(1.0);
        outputpressure << "\n";
      }
      {
        // write out flows:
        outputflow << times[i];
        auto var = values[i][1];
        outputflow << ",    " << var.at(0.0);
        outputflow << ",    " << var.at(1.0);
        outputflow << "\n";
      }
    }
  }

  int Shortcomponent::get_number_of_states() const {
    return number_of_state_variables;
  }

  void Shortcomponent::save_values(
      double time, Eigen::Ref<Eigen::VectorXd const> state) {
    std::map<double, double> pressure_map;
    std::map<double, double> flow_map;

    auto start_state = get_boundary_state(1, state);
    auto end_state = get_boundary_state(-1, state);

    pressure_map = {{0.0, start_state[0]}, {1.0, end_state[0]}};
    flow_map = {{0.0, start_state[1]}, {1.0, end_state[1]}};

    std::vector<std::map<double, double>> value_vector(
        {pressure_map, flow_map});
    Equationcomponent::push_to_values(time, value_vector);
  }

  void Shortcomponent::initial_values_helper(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) {
    if (get_start_state_index() == -1 or get_after_state_index() == -1) {
      gthrow(
          {"This function may only be called if set_indices  has been "
           "called beforehand!"});
    }

    auto start_p_index = get_boundary_state_index(1);
    auto start_q_index = start_p_index + 1;
    auto end_p_index = get_boundary_state_index(-1);
    auto end_q_index = end_p_index + 1;

    Initialvalue<2> initialvalues(initial_json);
    new_state[start_p_index] = initialvalues(0)[0];
    new_state[start_q_index] = initialvalues(0)[1];
    new_state[end_p_index] = initialvalues(1)[0];
    new_state[end_q_index] = initialvalues(1)[1];
  }

  Eigen::Vector2d Shortcomponent::get_boundary_p_qvol_bar(
      int direction, Eigen::Ref<Eigen::VectorXd const> state) const {
    return get_boundary_state(direction, state);
  }

  void Shortcomponent::dboundary_p_qvol_dstate(
      int direction, Aux::Matrixhandler *jacobianhandler,
      Eigen::RowVector2d function_derivative, int rootvalues_index,
      Eigen::Ref<Eigen::VectorXd const>) const {
    int p_index = get_boundary_state_index(direction);
    int q_index = p_index + 1;

    jacobianhandler->set_coefficient(
        rootvalues_index, p_index, function_derivative[0]);
    jacobianhandler->set_coefficient(
        rootvalues_index, q_index, function_derivative[1]);
  }

} // namespace Model::Networkproblem::Gas
