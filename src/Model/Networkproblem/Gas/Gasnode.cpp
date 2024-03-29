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
#include "Gasnode.hpp"
#include "Coloroutput.hpp"
#include "Edge.hpp"
#include "Exception.hpp"
#include "Gasedge.hpp"
#include "Matrixhandler.hpp"
#include <iostream>

namespace Model::Gas {

  Gasnode::~Gasnode() {}

  void Gasnode::evaluate_flow_node_balance(
      Eigen::Ref<Eigen::VectorXd> rootvalues,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      double prescribed_qvol) const {

    if (directed_attached_gas_edges.empty()) {
      return;
    }
    auto [dir0, edge0] = directed_attached_gas_edges.front();
    auto p_qvol0 = edge0->get_boundary_p_qvol_bar(dir0, state);
    auto p0 = p_qvol0[0];
    auto q0 = p_qvol0[1];

    double old_p = p0;
    auto old_equation_index = edge0->boundary_equation_index(dir0);

    // We will write the flow balance into the last index:
    auto last_direction = directed_attached_gas_edges.back().first;
    auto last_equation_index
        = directed_attached_gas_edges.back().second->boundary_equation_index(
            last_direction);

    // prescribed boundary condition is like an attached pipe ending at this
    // node...
    rootvalues[last_equation_index] = -1.0 * prescribed_qvol;
    rootvalues[last_equation_index] += static_cast<int>(dir0) * q0;

    // std::cout << "number of gas edges: " <<
    // directed_attached_gas_edges.size() <<std::endl;
    for (auto it = std::next(directed_attached_gas_edges.begin());
         it != directed_attached_gas_edges.end(); ++it) {
      auto direction = it->first;
      Gasedge *edge = it->second;
      auto current_p_qvol = edge->get_boundary_p_qvol_bar(direction, state);
      auto current_p = current_p_qvol[0];
      auto current_qvol = current_p_qvol[1];
      rootvalues[old_equation_index] = current_p - old_p;
      old_equation_index = edge->boundary_equation_index(direction);
      old_p = current_p;

      rootvalues[last_equation_index]
          += static_cast<int>(direction) * current_qvol;
    }
  }

  void Gasnode::evaluate_flow_node_derivative(
      Aux::Matrixhandler &jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> const &state) const {

    if (directed_attached_gas_edges.empty()) {
      return;
    }

    auto [dir0, edge0] = directed_attached_gas_edges.front();
    auto [dirlast, edgelast] = directed_attached_gas_edges.back();
    auto last_equation_index = edgelast->boundary_equation_index(dirlast);
    //    rootvalues[last_equation_index]=dir0*state0[1];

    Eigen::RowVector2d dF_last_dpq_0(0.0, dir0);
    edge0->dboundary_p_qvol_dstate(
        dir0, jacobianhandler, dF_last_dpq_0, last_equation_index, state);
    // if there is only one attached edge, we are done:
    if (edge0 == edgelast) {
      return;
    }

    // In all other cases we now have to make pressure derivatives and the other
    // flow derivatives:

    // first edge is special (sets only one p-derivative)
    Eigen::RowVector2d dF_0_dpq_0(-1.0, 0.0);
    auto old_equation_index = edge0->boundary_equation_index(dir0);

    edge0->dboundary_p_qvol_dstate(
        dir0, jacobianhandler, dF_0_dpq_0, old_equation_index, state);

    // first and last attached edge are special:
    auto second_iterator = std::next(directed_attached_gas_edges.begin());
    auto last_iterator = std::prev(directed_attached_gas_edges.end());
    for (auto it = second_iterator; it != last_iterator; ++it) {
      auto direction = it->first;
      Gasedge *edge = it->second;

      auto current_equation_index = edge->boundary_equation_index(direction);
      Eigen::RowVector2d dF_old_dpq_now(1.0, 0.0);
      Eigen::RowVector2d dF_now_dpq_now(-1.0, 0.0);
      Eigen::RowVector2d dF_last_dpq_now(0.0, direction);

      // Let the attached edge write out the derivative:
      edge->dboundary_p_qvol_dstate(
          direction, jacobianhandler, dF_old_dpq_now, old_equation_index,
          state);
      edge->dboundary_p_qvol_dstate(
          direction, jacobianhandler, dF_now_dpq_now, current_equation_index,
          state);
      edge->dboundary_p_qvol_dstate(
          direction, jacobianhandler, dF_last_dpq_now, last_equation_index,
          state);
      old_equation_index = current_equation_index;
    }
    // last edge:
    Eigen::RowVector2d dF_old_dpq_last(1.0, 0.0);
    edgelast->dboundary_p_qvol_dstate(
        dirlast, jacobianhandler, dF_old_dpq_last, old_equation_index, state);
    Eigen::RowVector2d dF_last_dpq_last(0.0, dirlast);
    edgelast->dboundary_p_qvol_dstate(
        dirlast, jacobianhandler, dF_last_dpq_last, last_equation_index, state);
  }

  void Gasnode::gasnode_setup_helper() {

    if (directed_attached_gas_edges.size() != 0) {
      std::cout << YELLOW << "You are calling setup a second time!" << RESET
                << std::endl;
    }
    directed_attached_gas_edges.clear();
    // std::cout << "number of start edges: " << get_starting_edges().size()
    // <<std::endl; std::cout << "number of end edges: " <<
    // get_ending_edges().size() <<std::endl;
    for (auto &startedge : get_starting_edges()) {
      auto startgasedge = dynamic_cast<Gasedge *>(startedge);
      if (!startgasedge) {
        std::cout << __FILE__ << ":" << __LINE__ << " Warning: The non-gasedge"
                  << startedge->get_id() << " is attached at node " << get_id()
                  << std::endl;
        std::cout << "node id: " << get_id() << std::endl;
        continue;
      }
      directed_attached_gas_edges.push_back({start, startgasedge});
    }
    for (auto &endedge : get_ending_edges()) {
      auto endgasedge = dynamic_cast<Gasedge *>(endedge);
      if (!endgasedge) {
        std::cout << __FILE__ << ":" << __LINE__ << " Warning: The non-gasedge"
                  << endedge->get_id() << " is attached at node " << get_id()
                  << std::endl;
        std::cout << "node id: " << get_id() << std::endl;
        continue;
      }
      directed_attached_gas_edges.push_back({end, endgasedge});
    }

    // Notify the user of unconnected nodes:
    if (directed_attached_gas_edges.empty()) {
      std::cout << "Node " << get_id() << " has no attached gas edges!"
                << std::endl;
      return;
    }
  }

} // namespace Model::Gas
