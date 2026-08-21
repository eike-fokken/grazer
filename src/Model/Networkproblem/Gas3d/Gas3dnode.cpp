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
#include "Gas3dnode.hpp"
#include "Coloroutput.hpp"
#include "Edge.hpp"
#include "Exception.hpp"
#include "Gas3dedge.hpp"
#include "Matrixhandler.hpp"
#include "MixedPipe.hpp"
#include <iostream>

namespace Model::Gas3d {

  Gas3dnode::~Gas3dnode() {}

  void Gas3dnode::evaluate_flow_node_balance(
      Eigen::Ref<Eigen::VectorXd> rootvalues,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      double prescribed_flow) const {

    if (directed_attached_gas_edges.empty()) {
      return;
    }

    // edges with direction and gas flow direction information:
    std::vector<std::tuple<Direction, bool /*is_outflowing*/, MixedPipe *>>
        flow_directed_pipes;

    for (auto const &[dir, gasedge] : directed_attached_gas_edges) {
      auto *pipe = dynamic_cast<MixedPipe *>(gasedge);

      auto outflowing
          = ((static_cast<double>(dir) * pipe->get_balancelaw().u(state)) > 0);
      flow_directed_pipes.push_back({dir, outflowing, pipe});
    }

    auto &[dir0, _outflowing0, pipe0] = flow_directed_pipes.front();

    auto boundary_state_0 = pipe0->get_boundary_state(dir0, state);
    auto q0 = boundary_state_0[2];
    auto p0 = pipe0->get_balancelaw().p(boundary_state_0);

    double old_p = p0;
    auto old_equation_index = pipe0->boundary_equation_index(dir0);

    // We will write the flow balance into the last index:
    auto last_direction = directed_attached_gas_edges.back().first;
    auto last_equation_index
        = directed_attached_gas_edges.back().second->boundary_equation_index(
            last_direction);

    // prescribed boundary condition is like an attached pipe ending at this
    // node...
    rootvalues[last_equation_index] = -1.0 * prescribed_flow;

    // As we only iterate over all later indices, we now set the flow
    // contribution of the first pipe:
    rootvalues[last_equation_index] += static_cast<int>(dir0) * q0;

    // std::cout << "number of gas edges: " <<
    // directed_attached_gas_edges.size() <<std::endl;
    for (auto it = std::next(flow_directed_pipes.begin());
         it != flow_directed_pipes.end(); ++it) {
      auto &[dir, _, pipe] = (*it);
      auto current_state = pipe->get_boundary_state(dir, state);
      auto current_p = pipe->get_balancelaw().p(current_state);
      auto current_q = current_state[2];
      rootvalues[old_equation_index] = current_p - old_p;
      old_equation_index = pipe->boundary_equation_index(dir);
      old_p = current_p;

      rootvalues[last_equation_index] += static_cast<int>(dir) * current_q;
    }
  }

  void Gas3dnode::evaluate_additional_outgoing_balance(
      Eigen::Ref<Eigen::VectorXd> rootvalues,
      Eigen::Ref<Eigen::VectorXd const> const &state, double prescribed_flow,
      double prescribed_component_1_share, bool boundary_node) const {}

  void Gas3dnode::evaluate_flow_node_derivative(
      Aux::Matrixhandler &jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> const &state) const {

    if (directed_attached_gas_edges.empty()) {
      return;
    }

    // edges with direction and gas flow direction information:
    std::vector<std::tuple<Direction, bool /*is_outflowing*/, MixedPipe *>>
        flow_directed_pipes;

    for (auto const &[dir, gasedge] : directed_attached_gas_edges) {
      auto *pipe = dynamic_cast<MixedPipe *>(gasedge);

      auto outflowing
          = ((static_cast<double>(dir) * pipe->get_balancelaw().u(state)) > 0);
      flow_directed_pipes.push_back({dir, outflowing, pipe});
    }

    auto &[dir0, _outflowing, pipe0] = flow_directed_pipes.front();
    auto &[dirlast, _last_outflowing, pipelast] = flow_directed_pipes.back();
    auto last_equation_index = pipelast->boundary_equation_index(dirlast);

    // rootvalues[last_equation_index] = -1.0 * prescribed_flow;
    // rootvalues[last_equation_index] += static_cast<int>(dir0) * q0;

    auto q0_index = pipelast->get_boundary_state_index(dirlast) + 2;
    jacobianhandler.add_to_coefficient(
        last_equation_index, q0_index, static_cast<double>(dir0));

    // if there is only one attached pipe, we are done:
    if (pipe0 == pipelast) {
      return;
    }

    // In all other cases we now have to make pressure derivatives and the
    // other flow derivatives:

    // first pipe is special (sets only one p-derivative)
    auto old_equation_index = pipe0->boundary_equation_index(dir0);
    auto old_state = pipe0->get_boundary_state(dir0, state);

    // equation is p_next - p_current = 0
    // This is the reason for the minus sign.
    Eigen::RowVector3d dF_0_dstate_0
        = -pipe0->get_balancelaw().dp_dstate(old_state);
    for (Eigen::Index i = 0; i != 3; ++i) {
      jacobianhandler.add_to_coefficient(
          old_equation_index, pipe0->get_boundary_state_index(dir0) + i,
          dF_0_dstate_0[i]);
    }

    // first and last attached pipe are special:
    auto second_iterator = std::next(flow_directed_pipes.begin());
    auto last_iterator = std::prev(flow_directed_pipes.end());
    for (auto it = second_iterator; it != last_iterator; ++it) {
      auto &[direction, _current_outflowing, pipe] = *it;

      auto current_equation_index = pipe->boundary_equation_index(direction);
      auto current_state = pipe->get_boundary_state(direction, state);
      Eigen::RowVector3d dF_old_dstate_now
          = pipe->get_balancelaw().dp_dstate(current_state);
      Eigen::RowVector3d dF_now_dstate_now
          = -pipe->get_balancelaw().dp_dstate(current_state);
      Eigen::RowVector3d dF_last_dstate_now(0.0, 0.0, direction);

      // Let the attached edge write out the derivative:

      for (Eigen::Index i = 0; i != 3; ++i) {
        jacobianhandler.add_to_coefficient(
            old_equation_index, pipe->get_boundary_state_index(direction) + i,
            dF_old_dstate_now[i]);
      }
      for (Eigen::Index i = 0; i != 3; ++i) {
        jacobianhandler.add_to_coefficient(
            current_equation_index,
            pipe->get_boundary_state_index(direction) + i,
            dF_now_dstate_now[i]);
      }

      for (Eigen::Index i = 0; i != 3; ++i) {
        jacobianhandler.add_to_coefficient(
            last_equation_index, pipe->get_boundary_state_index(direction) + i,
            dF_last_dstate_now[i]);
      }

      old_equation_index = current_equation_index;
    }
    // last edge:

    auto last_state = pipelast->get_boundary_state(dirlast, state);
    Eigen::RowVector3d dF_old_dstate_last
        = pipelast->get_balancelaw().dp_dstate(last_state);

    for (Eigen::Index i = 0; i != 3; ++i) {
      jacobianhandler.add_to_coefficient(
          old_equation_index, pipelast->get_boundary_state_index(dirlast) + i,
          dF_old_dstate_last[i]);
    }

    Eigen::RowVector3d dF_last_dstate_last(0.0, 0.0, dirlast);
    Eigen::RowVector2d dF_last_dpq_last(0.0, dirlast);

    for (Eigen::Index i = 0; i != 3; ++i) {
      jacobianhandler.add_to_coefficient(
          last_equation_index, pipelast->get_boundary_state_index(dirlast) + i,
          dF_last_dstate_last[i]);
    }
  }

  void Gas3dnode::evaluate_additional_outgoing_derivative(
      Aux::Matrixhandler &jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> const &state, double prescribed_flow,
      double prescribed_component_1_share, bool boundary_node) const {}

  void Gas3dnode::gasnode_setup_helper() {

    if (directed_attached_gas_edges.size() != 0) {
      std::cout << YELLOW << "You are calling setup a second time!" << RESET
                << std::endl;
    }
    directed_attached_gas_edges.clear();
    // std::cout << "number of start edges: " << get_starting_edges().size()
    // <<std::endl; std::cout << "number of end edges: " <<
    // get_ending_edges().size() <<std::endl;
    for (auto &startedge : get_starting_edges()) {
      auto startgasedge = dynamic_cast<Gas3dedge *>(startedge);
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
      auto endgasedge = dynamic_cast<Gas3dedge *>(endedge);
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

} // namespace Model::Gas3d
