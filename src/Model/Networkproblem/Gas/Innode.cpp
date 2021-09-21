#include <Innode.hpp>
#include <iostream>

namespace Model::Networkproblem::Gas {

  std::string Innode::get_type() { return "Innode"; }

  void Innode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state,
      Eigen::Ref<Eigen::VectorXd const> /*last_control*/,
      Eigen::Ref<Eigen::VectorXd const> /*new_control*/) const {
    evaluate_flow_node_balance(rootvalues, new_state, 0.0);
  }
  void Innode::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state,
      Eigen::Ref<Eigen::VectorXd const> /*last_control*/,
      Eigen::Ref<Eigen::VectorXd const> /*new_control*/) const {
    evaluate_flow_node_derivative(jacobianhandler, new_state);
  }

} // namespace Model::Networkproblem::Gas
