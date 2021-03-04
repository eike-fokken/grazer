#include <Exception.hpp>
#include <Pressureboundarynode.hpp>
#include <iostream>
#include <Eigen/Sparse>

namespace Model::Networkproblem::Gas {

  Pressureboundarynode::Pressureboundarynode(nlohmann::json const &data)
      : Gasnode(data) {
    boundaryvalue.set_boundary_condition(data["boundary_values"]);
  }

  void Pressureboundarynode::evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double ,
                double new_time, Eigen::Ref<Eigen::VectorXd const> const &,
                Eigen::Ref<Eigen::VectorXd const> const &new_state) const{

    if(directed_attached_gas_edges.empty()){ return; }
         evaluate_pressure_node_balance(rootvalues,new_state, boundaryvalue(new_time)[0]);
  }

  void Pressureboundarynode::evaluate_state_derivative(Aux::Matrixhandler *jacobianhandler,
                                 double , double ,
                                 Eigen::Ref<Eigen::VectorXd const> const &,
                                 Eigen::Ref<Eigen::VectorXd const> const &new_state) const{
    if(directed_attached_gas_edges.empty()){ return; }
      evaluate_pressure_node_derivative(jacobianhandler,new_state);
  }

}
