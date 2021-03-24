#include "make_schema.hpp"
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <Flowboundarynode.hpp>
#include <iostream>

namespace Model::Networkproblem::Gas {

  nlohmann::json Flowboundarynode::get_schema() {
    nlohmann::json schema = Gasnode::get_schema();
    schema["properties"]["boundary_values"]
        = Aux::schema::make_boundary_schema(1);
    return schema;
  }

  Flowboundarynode::Flowboundarynode(nlohmann::json const &data) :
      Gasnode(data), boundaryvalue(data["boundary_values"]) {}

  void Flowboundarynode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

    if (directed_attached_gas_edges.empty()) {
      return;
    }

    evaluate_flow_node_balance(
        rootvalues, new_state, boundaryvalue(new_time)[0]);
  }

  void Flowboundarynode::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    if (directed_attached_gas_edges.empty()) {
      return;
    }
    evaluate_flow_node_derivative(jacobianhandler, new_state);
  }

} // namespace Model::Networkproblem::Gas
