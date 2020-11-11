#include <Matrixhandler.hpp>
#include <Vphinode.hpp>

namespace Model::Networkproblem::Power {

  // Careful: new state and old state and function value needed!
  //////
  //////
  void Vphinode::evaluate(Eigen::VectorXd &rootfunction, double // last_time
                          ,
                          double next_time,
                          Eigen::VectorXd const & // last_state
                          ,
                          Eigen::VectorXd const &new_state) {
    auto index = get_start_state_index();
    rootfunction[index] = boundaryvalue(next_time)[0] - new_state[index];
    index++;
    rootfunction[index] = boundaryvalue(next_time)[1] - new_state[index];
  }

  void Vphinode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                           double // last_time
                                           ,
                                           double // new_time
                                           ,
                                           Eigen::VectorXd const &,
                                           Eigen::VectorXd const & // new_state
  ) {
    auto start_index = get_start_state_index();
    auto end_index = get_after_state_index();
    jacobianhandler->set_coefficient(start_index, start_index, -1.0);
    jacobianhandler->set_coefficient(end_index, end_index, -1.0);
  }

} // namespace Model::Networkproblem::Power
