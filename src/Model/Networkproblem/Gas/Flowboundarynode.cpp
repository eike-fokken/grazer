#include "make_schema.hpp"
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <Flowboundarynode.hpp>
#include <iostream>

namespace Model::Networkproblem::Gas {

  std::optional<nlohmann::json> Flowboundarynode::get_boundary_schema() {
    return Aux::schema::make_boundary_schema(1);
  }

  Flowboundarynode::Flowboundarynode(nlohmann::json const &data) :
      Gasnode(data), boundaryvalue(data["boundary_values"]) {}

  void Flowboundarynode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_control*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_control*/) const {

    if (directed_attached_gas_edges.empty()) {
      return;
    }

    evaluate_flow_node_balance(
        rootvalues, new_state, boundaryvalue(new_time)[0]);
  }

  void Flowboundarynode::d_evalutate_d_new_state(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_control*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_control*/) const {
    if (directed_attached_gas_edges.empty()) {
      return;
    }
    evaluate_flow_node_derivative(jacobianhandler, new_state);
  }

} // namespace Model::Networkproblem::Gas
