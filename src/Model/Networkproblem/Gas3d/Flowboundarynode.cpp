/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#include "make_schema.hpp"
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <Flowboundarynode.hpp>
#include <iostream>

namespace Model::Gas3d {

  void Flowboundarynode::setup() { gasnode_setup_helper(); }

  void Flowboundarynode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

    if (directed_attached_gas_edges.empty()) {
      return;
    }

    auto prescribed_share = prescribed_component_1_share(new_time);

    if (prescribed_share < 0 or prescribed_share > 1) {
      gthrow(
          {"Component1 share is outside of [0,1 in Gas3dnode with id:\n",
           get_id(), "\nat time:\n", std::to_string(new_time),
           "This is not feasible!"});
    }

    evaluate_flow_node_balance(
        rootvalues, new_state, prescribed_flow_value(new_time),
        prescribed_share);
  }

  void Flowboundarynode::d_evaluate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    if (directed_attached_gas_edges.empty()) {
      return;
    }
    evaluate_flow_node_derivative(jacobianhandler, new_state);
  }

  void Flowboundarynode::d_evaluate_d_last_state(
      Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const {}

} // namespace Model::Gas3d
