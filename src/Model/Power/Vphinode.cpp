#include <Matrixhandler.hpp>
#include <Vphinode.hpp>

namespace Model::Networkproblem::Power {

  // Careful: new state and old state and function value needed!
  //////
  //////
  void Vphinode::evaluate(Eigen::VectorXd &rootfunction, double // last_time
                          ,
                          double new_time,
                          Eigen::VectorXd const & // last_state
                          ,
                          Eigen::VectorXd const &new_state) {
    int index1 = get_start_state_index();
    int index2 = index1 + 1;
    rootfunction[index1] = new_state[index1] - boundaryvalue(new_time)[0];

    rootfunction[index2] = new_state[index2] - boundaryvalue(new_time)[1];
  }

  void Vphinode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                           double // last_time
                                           ,
                                           double // new_time
                                           ,
                                           Eigen::VectorXd const &,
                                           Eigen::VectorXd const & // new_state
  ) {
    int index1 = get_start_state_index();
    int index2 = index1 + 1;
    jacobianhandler->set_coefficient(index1, index1, 1.0);
    jacobianhandler->set_coefficient(index2, index2, 1.0);
  }

  void Vphinode::display() {
    std::cout << "type: Vphi, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
