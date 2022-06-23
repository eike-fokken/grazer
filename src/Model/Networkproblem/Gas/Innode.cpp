#include <Innode.hpp>
#include <iostream>

namespace Model::Gas {

  std::string Innode::get_type() { return "Innode"; }

  void Innode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    evaluate_flow_node_balance(rootvalues, new_state, 0.0);
  }
  void Innode::d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    evaluate_flow_node_derivative(jacobianhandler, new_state);
  }

  void Innode::d_evalutate_d_last_state(
      Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const {}

} // namespace Model::Gas
