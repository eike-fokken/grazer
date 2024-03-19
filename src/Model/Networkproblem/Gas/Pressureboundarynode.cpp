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
#include "Pressureboundarynode.hpp"
#include "Exception.hpp"
#include "make_schema.hpp"
#include <Eigen/Sparse>
#include <iostream>

namespace Model::Gas {

  std::string Pressureboundarynode::get_type() {
    return "Pressureboundarynode";
  }

  nlohmann::json Pressureboundarynode::get_boundary_schema() {
    return Aux::schema::make_boundary_schema(1);
  }

  Pressureboundarynode::Pressureboundarynode(nlohmann::json const &data) :
      Gasnode(data),
      boundaryvalue(Aux::InterpolatingVector::construct_from_json(
          data["boundary_values"], get_boundary_schema())) {}

  void Pressureboundarynode::setup() { gasnode_setup_helper(); }

  void Pressureboundarynode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

    if (directed_attached_gas_edges.empty()) {
      return;
    }

    auto prescribed_p = boundaryvalue(new_time)[0];

    for (auto const &[direction, edge_ptr] : directed_attached_gas_edges) {
      auto const &edge = *edge_ptr;
      auto p_qvol = edge.get_boundary_p_qvol_bar(direction, new_state);
      auto p = p_qvol[0];
      auto equation_index = edge.boundary_equation_index(direction);
      rootvalues[equation_index] = p - prescribed_p;
    }
  }

  void Pressureboundarynode::d_evaluate_d_new_state(
      Aux::Matrixhandler &jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const {
    if (directed_attached_gas_edges.empty()) {
      return;
    }

    for (auto const &[direction, edge_ptr] : directed_attached_gas_edges) {
      auto const &edge = *edge_ptr;
      auto equation_index = edge.boundary_equation_index(direction);
      Eigen::RowVector2d dF_now_dpq_now(1.0, 0.0);

      // Let the attached edge write out the derivative:
      edge.dboundary_p_qvol_dstate(
          direction, jacobianhandler, dF_now_dpq_now, equation_index,
          new_state);
    }
  }

  void Pressureboundarynode::d_evaluate_d_last_state(
      Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
      double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const {}

} // namespace Model::Gas
