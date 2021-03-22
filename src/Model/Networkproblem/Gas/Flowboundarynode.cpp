#include <Exception.hpp>
#include <Flowboundarynode.hpp>
#include <iostream>
#include <Eigen/Sparse>

namespace Model::Networkproblem::Gas {

  Flowboundarynode::Flowboundarynode(nlohmann::json const &data)
      : Gasnode(data), boundaryvalue(data["boundary_values"]) {}

  void Flowboundarynode::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double ,
                double new_time, Eigen::Ref<Eigen::VectorXd const> const &,
                Eigen::Ref<Eigen::VectorXd const> const &new_state) const{

    if(directed_attached_gas_edges.empty()){ return; }

        evaluate_flow_node_balance(rootvalues,new_state, boundaryvalue(new_time)[0]);
      }

  void Flowboundarynode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                 double , double ,
                                 Eigen::Ref<Eigen::VectorXd const> const &,
                                 Eigen::Ref<Eigen::VectorXd const> const &new_state) const{
    if(directed_attached_gas_edges.empty()){ return; }
    evaluate_flow_node_derivative(jacobianhandler,new_state);
  }


}
