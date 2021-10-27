#include "make_schema.hpp"
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <Flowboundarynode.hpp>
#include <iostream>

namespace Model::Gas {

  nlohmann::json revert_boundary_conditions(nlohmann::json const &data) {

    nlohmann::json reverted_boundary_json = data;
    if (not reverted_boundary_json.contains("boundary_values")) {
      gthrow(
          {"The sink json ", data["id"],
           " did not contain boundary values.  Something went wrong.\n",
           "Its json was: ", data.dump(1, '\t')});
    }
    for (auto &datum : reverted_boundary_json["boundary_values"]["data"]) {
      for (auto &value_json : datum["values"]) {
        double old_value = value_json.get<double>();
        value_json = -old_value;
      }
    }

    return reverted_boundary_json;
  }
  nlohmann::json Flowboundarynode::get_boundary_schema() {
    return Aux::schema::make_boundary_schema(1);
  }

  Flowboundarynode::Flowboundarynode(nlohmann::json const &data) :
      Gasnode(data),
      boundaryvalue(Aux::InterpolatingVector::construct_from_json(
          data["boundary_values"], get_boundary_schema())) {}

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

  void Flowboundarynode::d_evalutate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    if (directed_attached_gas_edges.empty()) {
      return;
    }
    evaluate_flow_node_derivative(jacobianhandler, new_state);
  }

  void Flowboundarynode::d_evalutate_d_last_state(
      Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const {}

} // namespace Model::Gas
