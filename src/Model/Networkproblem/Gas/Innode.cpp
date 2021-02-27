#include "Bernoulligasnode.hpp"
#include <Innode.hpp>
#include <iostream>

namespace Model::Networkproblem::Gas {

  template<typename Couplingnode>
  std::string Innode<Couplingnode>::get_type() { return "Innode"; }

  template <typename Couplingnode>
  void Innode<Couplingnode>::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    this->evaluate_flow_node_balance(rootvalues, new_state, 0.0);
  }

  template <typename Couplingnode>
  void Innode<Couplingnode>::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    this->evaluate_flow_node_derivative(jacobianhandler, new_state);
  }

  template <typename Couplingnode>
  void Innode<Couplingnode>::display() const {}

  template class Innode<Gasnode>;
  template class Innode<Bernoulligasnode>;
}
