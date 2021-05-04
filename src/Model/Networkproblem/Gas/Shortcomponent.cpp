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

  void Shortcomponent::json_save(
      nlohmann::json &output, double time,
      Eigen::Ref<Eigen::VectorXd const> state) {
    nlohmann::json &current_component_vector = output[get_gas_type()];

    auto id = get_id_copy();

    // define a < function as a lambda:
    auto id_compare_less
        = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
      return a["id"].get<std::string>() < b["id"].get<std::string>();
    };

    // auto id_is = [id](nlohmann::json const &a) -> bool {
    //   return a["id"].get<std::string>() == id;
    // };
    nlohmann::json this_id;
    this_id["id"] = id;
    auto it = std::lower_bound(
        current_component_vector.begin(), current_component_vector.end(),
        this_id, id_compare_less);

    nlohmann::json current_value;
    current_value["time"] = time;
    current_value["pressure"] = nlohmann::json::array();
    auto start_state = get_boundary_state(1, state);
    auto end_state = get_boundary_state(-1, state);
    nlohmann::json pressure0_json;
    pressure0_json["x"] = 0.0;
    pressure0_json["value"] = start_state[0];
    current_value["pressure"].push_back(pressure0_json);

    nlohmann::json pressure1_json;
    pressure1_json["x"] = 1.0;
    pressure1_json["value"] = end_state[0];
    current_value["pressure"].push_back(pressure1_json);
    nlohmann::json flow0_json;
    flow0_json["x"] = 0.0;
    flow0_json["value"] = start_state[1];
    current_value["flow"].push_back(flow0_json);

    nlohmann::json flow1_json;
    flow1_json["x"] = 1.0;
    flow1_json["value"] = end_state[1];
    current_value["flow"].push_back(flow1_json);

    if (it == current_component_vector.end()) {
      // std::cout << "Not found!" << std::endl;
      nlohmann::json newoutput;
      newoutput["id"] = get_id_copy();
      newoutput["data"] = nlohmann::json::array();
      newoutput["data"].push_back(current_value);
      current_component_vector.push_back(newoutput);
    } else {
      if ((*it)["id"] != id) {
        gthrow(
            {"The json value\n", (*it).dump(4),
             "\n has an id different from the current object, whose id is ", id,
             "\n This is a bug. Please report it!"});
      }

      // std::cout << "found!" << std::endl;
      nlohmann::json &outputjson = (*it);
      outputjson["data"].push_back(current_value);
    }
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
