#include <Matrixhandler.hpp>
#include <PQnode.hpp>
#include <Transmissionline.hpp>
#include <cmath>
#include <math.h>

namespace Model::Networkproblem::Power {

  void PQnode::evaluate(Eigen::VectorXd &rootfunction, double, double new_time,
                        Eigen::VectorXd const &,
                        Eigen::VectorXd const &new_state) {
    int index1 = get_start_state_index();
    int index2 = index1 + 1;
    rootfunction[index1] = P(new_time, new_state);
    rootfunction[index2] = Q(new_time, new_state);
  }

  void PQnode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                         double // last_time
                                         ,
                                         double // new_time
                                         ,
                                         Eigen::VectorXd const &,
                                         Eigen::VectorXd const &new_state) {
    evaluate_P_derivative(jacobianhandler, new_state);
    evaluate_Q_derivative(jacobianhandler, new_state);
  }

  void PQnode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "type: PQ, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
