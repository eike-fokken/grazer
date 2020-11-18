#include <Matrixhandler.hpp>
#include <PVnode.hpp>
#include <Transmissionline.hpp>

namespace Model::Networkproblem::Power {

  void PVnode::evaluate(Eigen::VectorXd &rootfunction, double, double new_time,
                        Eigen::VectorXd const &,
                        Eigen::VectorXd const &new_state) {
    int index1 = get_start_state_index();
    int index2 = index1 + 1;
    rootfunction[index1] = P(new_time, new_state);
    rootfunction[index2] = new_state[index2] - boundaryvalue(new_time)[1];
  }

  void PVnode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                         double // last_time
                                         ,
                                         double // new_time
                                         ,
                                         Eigen::VectorXd const &,
                                         Eigen::VectorXd const &new_state) {
    int index2 = get_start_state_index() + 1;
    evaluate_P_derivative(jacobianhandler, new_state);
    jacobianhandler->set_coefficient(index2, index2, 1.0);
  }

  void PVnode::display() const {
    Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "type: PV, G: " << G << ", B: " << B << "\n";
  }

} // namespace Model::Networkproblem::Power
