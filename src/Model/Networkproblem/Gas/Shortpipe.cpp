#include <Shortpipe.hpp>
#include <Matrixhandler.hpp>


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

  void Shortpipe::set_initial_values(Eigen::VectorXd &new_state,
                                     nlohmann::ordered_json initial_json) {
    
  }
}
