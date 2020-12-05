#include <Implicitboxscheme.hpp>
#include <Isothermaleulerequation.hpp>
#include <Matrixhandler.hpp>
#include <Eigen/Dense>
#include <Pipe.hpp>
#include <cmath>
#include <Mathfunctions.hpp>
#include <Initialvalue.hpp>
#include <fstream>

namespace Model::Networkproblem::Gas {

  Pipe::Pipe(std::string _id, Network::Node *start_node,
             Network::Node *end_node, nlohmann::ordered_json topology_json,
             double _Delta_x)
      : Gasedge(_id, start_node, end_node),
        length(topology_json["length"]["value"].get<double>() * 1e3),
        diameter(topology_json["diameter"]["value"].get<double>() * 1e-3),
        roughness(topology_json["roughness"]["value"].get<double>()),
        number_of_points(static_cast<int> (std::ceil(length/_Delta_x))+1),
        Delta_x(length/(number_of_points-1)),
        bl(Balancelaw::Isothermaleulerequation(Aux::circle_area(0.5 * diameter),
                                               diameter, roughness)) {}


  int Pipe::get_number_of_states() const {
    return 2*number_of_points;
  }


  void
  Pipe::print_to_files(std::filesystem::path const &output_directory) {
    std::filesystem::path pipe_output_pressure(output_directory / (get_id().insert(0, "Gas_Pipe_p_")));
    std::filesystem::path pipe_output_flow( output_directory / (get_id().insert(0, "Gas_Pipe_q_")));

    std::ofstream outputpressure(pipe_output_pressure);
    std::ofstream outputflow(pipe_output_flow);

    auto times = get_times();
    auto values = get_values();


    outputpressure << "t-x";
    for (int i = 0;i!=number_of_points;++i){
      outputpressure <<",\t" << std::to_string(i*Delta_x);
      outputflow <<",\t" << std::to_string(i*Delta_x);
    }
    outputpressure << "\n";
    outputflow << "\n";

    for (unsigned i = 0; i != times.size(); ++i) {
      outputpressure << times[i];
      for(auto & [x,pressure] : values[i][0]){
        outputpressure << ",\t " << pressure;
      }
      outputpressure << "\n";

      outputflow << times[i];
      for(auto & [x,flow] : values[i][1]){
        outputflow << ",\t " << flow;
      }
      outputflow << "\n";
    }
  }



  void Pipe::save_values(double time, Eigen::VectorXd const &state) {
    std::map<double, double> pressure_map;
    std::map<double, double> flow_map;

    for (int i = 0; i!=number_of_points;++i){
      Eigen::Vector2d current_state= state.segment<2>(get_start_state_index()+2*i);
      double current_rho = current_state[0];
      double current_p_bar = bl.p_bar_from_p_pascal(bl.p(current_rho));
      double current_q = current_state[1];
      double x = i*Delta_x;
      pressure_map.insert(pressure_map.end(),{x,current_p_bar});
      flow_map.insert(flow_map.end(),{x,current_q});
    }
    
    std::vector<std::map<double, double>> value_vector({pressure_map, flow_map});
    Equationcomponent::push_to_values(time, value_vector);

  }

  void Pipe::set_initial_values(Eigen::VectorXd &new_state,
                                nlohmann::ordered_json initial_json) {
    Initialvalue<Pipe,2> initialvalues;
    initialvalues.set_initial_condition(initial_json);
    for (int i = 0; i!=number_of_points;++i){
      new_state.segment<2>(get_start_state_index()+2*i) = bl.state(bl.p_qvol_from_p_qvol_bar(initialvalues(i*Delta_x)));
    }
  }





  Eigen::Vector2d
  Pipe::get_boundary_p_qvol(int direction, Eigen::VectorXd const &state) const {
    Eigen::Vector2d b_state = get_boundary_state( direction, state);

    return bl.p_qvol(b_state);
  }

  void Pipe::dboundary_p_qvol_dstate(int direction,
                               Aux::Matrixhandler *jacobianhandler,
                               Eigen::RowVector2d function_derivative,
                               int rootvalues_index,
                                     Eigen::VectorXd const &state) const {
    Eigen::RowVector2d derivative;
    derivative = function_derivative *
                 bl.dp_qvol_dstate(get_boundary_state(direction, state));
    int rho_index = get_boundary_state_index(direction);
    int q_index = rho_index + 1;
    jacobianhandler->set_coefficient(rootvalues_index, rho_index,
                                     derivative[0]);
    jacobianhandler->set_coefficient(rootvalues_index, q_index, derivative[1]);
  }

} // namespace Model::Networkproblem::Gas
