#include "unit_conversion.hpp"
#include "Edge.hpp"
#include <Coloroutput.hpp>
#include <Eigen/Dense>
#include <Implicitboxscheme.hpp>
#include <Initialvalue.hpp>
#include <Isothermaleulerequation.hpp>
#include <Mathfunctions.hpp>
#include <Matrixhandler.hpp>
#include <Pipe.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>


namespace Model::Networkproblem::Gas {

  std::string Pipe::get_type(){return "Pipe";}

  Pipe::Pipe(nlohmann::json const &topology,
             std::vector<std::unique_ptr<Network::Node>> &nodes)
      : Network::Edge(topology, nodes),
        length(Aux::unit::parse_to_si(topology["length"],
                                      Aux::unit::length_units)),
        diameter(Aux::unit::parse_to_si(topology["diameter"],
                                        Aux::unit::length_units)),
        roughness(Aux::unit::parse_to_si(topology["roughness"],
                                         Aux::unit::length_units)),
        number_of_points(
            static_cast<int>(std::ceil(
                length /
                std::stod(topology["desired_delta_x"].get<std::string>()))) +
            1),
        Delta_x(length / (number_of_points - 1)) {}

  void Pipe::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    for (int i = get_equation_start_index(); i!=get_equation_after_index();i+=2){

      Eigen::Ref<Eigen::Vector2d> rootvalue_segment = rootvalues.segment<2>(i);
      
      auto last_left = last_state.segment<2>(i - 1);
      auto last_right = last_state.segment<2>(i + 1);
      auto new_left = new_state.segment<2>(i - 1);
      auto new_right = new_state.segment<2>(i + 1);
      
      scheme.evaluate_point(rootvalue_segment, last_time, new_time,
           Delta_x, last_left,
           last_right, new_left, new_right, bl,diameter,roughness);
    }
  }

  void Pipe::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                       double last_time, double new_time,
                                       Eigen::Ref<Eigen::VectorXd const> const &last_state,
                                       Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    for (int i = get_equation_start_index(); i != get_equation_after_index();
         i += 2) {
      // maybe use Eigen::Ref here to avoid copies.
      auto last_left = last_state.segment<2>(i - 1);
      auto last_right = last_state.segment<2>(i + 1);
      auto new_left = new_state.segment<2>(i - 1);
      auto new_right = new_state.segment<2>(i + 1);

      Eigen::Matrix2d current_derivative_left = scheme.devaluate_point_dleft(
          last_time, new_time, Delta_x, last_left, last_right, new_left,
          new_right, bl, diameter, roughness);

      jacobianhandler->set_coefficient(i,i-1 ,current_derivative_left(0,0));
      jacobianhandler->set_coefficient(i, i , current_derivative_left(0, 1));
      jacobianhandler->set_coefficient(i+1, i - 1, current_derivative_left(1, 0));
      jacobianhandler->set_coefficient(i+1, i, current_derivative_left(1, 1));

      Eigen::Matrix2d current_derivative_right = scheme.devaluate_point_dright(
          last_time, new_time, Delta_x, last_left, last_right, new_left,
          new_right, bl, diameter, roughness);

      jacobianhandler->set_coefficient(i, i + 1, current_derivative_right(0, 0));
      jacobianhandler->set_coefficient(i, i+2, current_derivative_right(0, 1));
      jacobianhandler->set_coefficient(i + 1, i + 1, current_derivative_right(1, 0));
      jacobianhandler->set_coefficient(i + 1, i+ 2, current_derivative_right(1, 1));
    }
  }

  int Pipe::get_number_of_states() const {
    return 2*number_of_points;
  }


  void
  Pipe::print_to_files(std::filesystem::path const &output_directory) {

    std::string pressure_file_name = (get_id_copy().insert(0, "Gas_Pipe_"))+"_p";
    std::string flow_file_name =(get_id_copy().insert(0, "Gas_Pipe_"))+"_q";
    std::filesystem::path shortpipe_output_pressure(output_directory /
                                                    pressure_file_name);
    std::filesystem::path shortpipe_output_flow(
                                                output_directory / flow_file_name);

    std::ofstream outputpressure(shortpipe_output_pressure);
    std::ofstream outputflow(shortpipe_output_flow);

    auto times = get_times();
    auto values = get_values();

    outputpressure.precision(9);
    outputflow.precision(9);

    outputpressure << "t-x";
    outputflow << "t-x";
    for (int i = 0;i!=number_of_points;++i){
      outputpressure <<",   " << std::to_string(i*Delta_x);
      outputflow <<",   " << std::to_string(i*Delta_x);
    }
    outputpressure << "\n";
    outputflow << "\n";

    for (unsigned i = 0; i != times.size(); ++i) {
      outputpressure << times[i];
      for(auto & [x,pressure] : values[i][0]){
        outputpressure << ",    " << pressure;
      }
      outputpressure << "\n";

      outputflow << times[i];
      for(auto & [x,flow] : values[i][1]){
        outputflow << ",    " << flow;
      }
      outputflow << "\n";
    }
  }



  void Pipe::save_values(double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
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
    Equationcomponent::push_to_values(time,  value_vector);

  }

  void Pipe::set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                                nlohmann::ordered_json initial_json) {
    Initialvalue<2> initialvalues(initial_json);
    for (int i = 0; i!=number_of_points;++i){
      try{
      new_state.segment<2>(get_start_state_index()+2*i) = bl.state(bl.p_qvol_from_p_qvol_bar(initialvalues(i*Delta_x)));
      } catch(...){
        std::cout<< "could not set initial value of pipe " << get_id() << ".\n"
                 << "Requested point was " << i*Delta_x << ". \n" <<
          "Length of line is " << length <<std::endl;
        std::cout << "requested - length: " << (i*Delta_x - length) << std::endl;
        throw;
      }
    }
  }





  Eigen::Vector2d
  Pipe::get_boundary_p_qvol_bar(int direction, Eigen::Ref<Eigen::VectorXd const> const &state) const {
    Eigen::Vector2d b_state = get_boundary_state(direction, state);
    Eigen::Vector2d p_qvol = bl.p_qvol(b_state);
    return bl.p_qvol_bar_from_p_qvol(p_qvol);
  }

  void Pipe::dboundary_p_qvol_dstate(int direction,
                               Aux::Matrixhandler *jacobianhandler,
                               Eigen::RowVector2d function_derivative,
                               int rootvalues_index,
                                     Eigen::Ref<Eigen::VectorXd const> const &state) const {
    Eigen::Vector2d boundary_state = get_boundary_state(direction,state);

        Eigen::Vector2d p_qvol = bl.p_qvol(boundary_state);

    Eigen::Matrix2d dp_qvol_dstate = bl.dp_qvol_dstate(boundary_state);
    Eigen::Matrix2d dpqvolbar_dpqvol =
        bl.dp_qvol_bar_from_p_qvold_p_qvol(p_qvol);

    Eigen::Matrix2d dpqvol_bar_dstate = dpqvolbar_dpqvol*dp_qvol_dstate;

    Eigen::RowVector2d derivative;
    derivative = function_derivative * dpqvol_bar_dstate;


    int rho_index = get_boundary_state_index(direction);
    int q_index = rho_index + 1;
    jacobianhandler->set_coefficient(rootvalues_index, rho_index,
                                     derivative[0]);
    jacobianhandler->set_coefficient(rootvalues_index, q_index, derivative[1]);
  }

} // namespace Model::Networkproblem::Gas
