#include <Shortpipe.hpp>
#include <Matrixhandler.hpp>
#include <Exception.hpp>
#include <iostream>
#include <fstream>

namespace Model::Networkproblem::Gas {

  void Shortpipe::evaluate(Eigen::VectorXd &rootfunction, double ,
                double , Eigen::VectorXd const &,
                Eigen::VectorXd const &new_state) const {
    rootfunction.segment<2>(get_start_state_index()) =
      get_starting_state(new_state) - get_ending_state(new_state);
  }

  void Shortpipe::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double , double ,
      Eigen::VectorXd const &, Eigen::VectorXd const &) const {

    auto start_p_index=get_starting_state_index();
    auto start_q_index = start_p_index+1;
    auto end_p_index = get_ending_state_index();
    auto end_q_index = end_p_index + 1;

    auto start_equation_index = get_equation_start_index();
    auto end_equation_index = start_equation_index+1;

    jacobianhandler->set_coefficient(start_equation_index, start_p_index, 1.0);
    jacobianhandler->set_coefficient(start_equation_index, end_p_index, -1.0);
    jacobianhandler->set_coefficient(end_equation_index, start_q_index, 1.0);
    jacobianhandler->set_coefficient(end_equation_index, end_q_index, -1.0);
  }


  int Shortpipe::get_number_of_states() const { return 4; }

  void Shortpipe::save_values(double time, Eigen::VectorXd const &state) {

    std::map<double, double> pressure_map;
    std::map<double, double> flow_map;

    auto start_state= get_starting_state(state);
    auto end_state = get_ending_state(state);

    pressure_map = {{0.0, start_state[0]},{1.0, end_state[0]}};
    flow_map = {{0.0, start_state[1]}, {1.0, end_state[1]}};

    std::vector<std::map<double, double>> value_vector({pressure_map, flow_map});
    Equationcomponent::push_to_values(time, value_vector);
  }

  void Shortpipe::set_initial_values(Eigen::VectorXd &new_state,
                                     nlohmann::ordered_json initial_json) {


      if (get_start_state_index() == -1) {
        gthrow({"This function may only be called if set_indices  has been "
                "called beforehand!"});
      }

      if (initial_json["data"]["value"].size() != 2 || initial_json["data"].size()!=2) {
        std::cout << "The initial json for this shortpipe is given by:"
                  << "\n";
        std::cout << initial_json << std::endl;
        gthrow({"This is not a shortpipe initial condition!"});
      }
      auto start_p_index = get_starting_state_index();
      auto start_q_index = start_p_index + 1;
      auto end_p_index = get_ending_state_index();
      auto end_q_index = end_p_index + 1;

      new_state[start_p_index] = initial_json["data"][0]["value"][0];
      new_state[start_q_index] = initial_json["data"][0]["value"][1];
      new_state[end_p_index] = initial_json["data"][1]["value"][0];
      new_state[end_q_index] = initial_json["data"][1]["value"][1];
  }
}
