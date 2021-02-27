#include "Bernoulligasnode.hpp"
#include "Exception.hpp"
#include "Flowboundarynode.hpp"
#include <iostream>
#include <Eigen/Sparse>

namespace Model::Networkproblem::Gas {

  template<typename Couplingnode>  
  Flowboundarynode<Couplingnode>::Flowboundarynode(nlohmann::json const &data)
    : Couplingnode(data) {
    boundaryvalue.set_boundary_condition(data["boundary_values"]);
  }
  template <typename Couplingnode>
  void Flowboundarynode<Couplingnode>::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

    
    this->evaluate_flow_node_balance(rootvalues,new_state, boundaryvalue(new_time)[0]);
  }

  template <typename Couplingnode>
  void Flowboundarynode<Couplingnode>::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    this->evaluate_flow_node_derivative(jacobianhandler,new_state);
  }

  template <typename Couplingnode>
  void Flowboundarynode<Couplingnode>::display() const {
    Network::Node::print_id();
    Equationcomponent::print_indices();
    std::cout << "NO FURTHER DISPLAY PARAMETERS IMPLEMENTED IN" << __FILE__ << ":" << __LINE__ << std::endl;
    // here show information of the gas node!
  }

  template class Flowboundarynode<Gasnode>;
  template class Flowboundarynode<Bernoulligasnode>;
}
