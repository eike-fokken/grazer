#include "Bernoulligasnode.hpp"
#include "Coloroutput.hpp"
#include "Edge.hpp"
#include "Exception.hpp"
#include "Gasedge.hpp"
#include "Matrixhandler.hpp"
#include "Pipe.hpp"
#include <Eigen/src/Core/Matrix.h>
#include <iostream>

namespace Model::Networkproblem::Gas {

  /// \brief computes the Bernoulli invariant.
  double bernoulli(Eigen::Vector2d p_qvol_bar, Pipe const &pipe) {

    auto bl = pipe.bl;
    double p = p_qvol_bar[0] * bl.bar;
    double q = p_qvol_bar[1];

    double area = pipe.bl.Area;
    double v = bl.rho_0 * q / (area * bl.rho(p));
    double pressure_part =
        1 / bl.c_vac_squared * (log(p / bl.p_0) + bl.alpha * (p - bl.p_0));

    return 0.5 * v * v + pressure_part;
  }

  /// \brief computes the derivative of the bernoulli invariant.
  ///
  /// Be careful: The derivative is with respect to pressure measured in bar!
  Eigen::RowVector2d dbernoulli_dstate(Eigen::Vector2d p_qvol_bar,
                                       Pipe const &pipe) {

    auto bl = pipe.bl;
    double p = p_qvol_bar[0] * bl.bar;
    double q = p_qvol_bar[1];
    double area = pipe.bl.Area;
    double v = bl.rho_0 * q / (area * bl.rho(p));
    auto db_dp = 1.0/bl.bar*
        (-v * v / bl.c_vac_squared / (bl.rho(p) * (1.0 + bl.alpha * p)) +
         1.0 / bl.c_vac_squared * (1.0 / p + bl.alpha));

    double stuff = bl.rho_0 / (area * bl.rho(p));
    auto db_dq = stuff * stuff * q;

    return Eigen::RowVector2d(db_dp, db_dq);
  }

  void Bernoulligasnode::evaluate_flow_node_balance(
      Eigen::Ref<Eigen::VectorXd> rootvalues,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      double prescribed_qvol) const {

    if (directed_non_pipe_gas_edges.empty() and directed_pipes.empty()) {
      return;
    }

    // We will write the flow balance into the last index:
    // if non-pipes are present, take the last of them.
    // Else take the last pipe.

    int last_equation_index;
    if (!directed_non_pipe_gas_edges.empty()) {
      int last_direction = directed_non_pipe_gas_edges.back().first;
      last_equation_index =
          directed_non_pipe_gas_edges.back().second->give_away_boundary_index(
              last_direction);
    } else { // Here pipes cannot be empty!
      int last_direction = directed_pipes.back().first;
      last_equation_index =
          directed_pipes.back().second->give_away_boundary_index(
              last_direction);
    }
    // add the boundary condition:
    rootvalues[last_equation_index] = -1.0 * prescribed_qvol;

    // deal with pipes:
    if (!directed_pipes.empty()) {
      auto [dirpipe0, pipe0] = directed_pipes.front();
      auto p_qvol0 = pipe0->get_boundary_p_qvol_bar(dirpipe0, state);
      auto q0 = p_qvol0[1];

      double old_H = bernoulli(p_qvol0, *pipe0);
      int old_equation_index = pipe0->give_away_boundary_index(dirpipe0);

      rootvalues[last_equation_index] += dirpipe0 * q0;

      for (auto it = std::next(directed_pipes.begin());
           it != directed_pipes.end(); ++it) {
        auto [pipedir, pipe] = *it;
        auto current_p_qvol = pipe->get_boundary_p_qvol_bar(pipedir, state);
        auto current_H = bernoulli(current_p_qvol, *pipe);
        auto current_qvol = current_p_qvol[1];
        rootvalues[old_equation_index] = current_H - old_H;
        rootvalues[last_equation_index] += pipedir * current_qvol;

        old_equation_index = pipe->give_away_boundary_index(pipedir);
        old_H = current_H;
      }
    }

    // if there are non-pipes present:
    if (!directed_non_pipe_gas_edges.empty()) {
      auto [dir0, edge0] = directed_non_pipe_gas_edges.front();
      auto p_qvol0 = edge0->get_boundary_p_qvol_bar(dir0, state);
      auto p0 = p_qvol0[0];
      auto q0 = p_qvol0[1];

      double old_p = p0;
      int old_equation_index = edge0->give_away_boundary_index(dir0);

      rootvalues[last_equation_index] += dir0 * q0;

      for (auto it = std::next(directed_non_pipe_gas_edges.begin());
           it != directed_non_pipe_gas_edges.end(); ++it) {
        auto [edgedir, edge] = *it;
        auto current_p_qvol = edge->get_boundary_p_qvol_bar(edgedir, state);
        auto current_p = current_p_qvol[0];
        auto current_qvol = current_p_qvol[1];
        rootvalues[old_equation_index] = current_p - old_p;
        rootvalues[last_equation_index] += edgedir * current_qvol;

        old_equation_index = edge->give_away_boundary_index(edgedir);
        old_p = current_p;
      }
    }
    // In the following case the equation index of the last pipe was not used!
    if (!directed_non_pipe_gas_edges.empty() and !directed_pipes.empty()) {
      auto [lastpipedir, lastpipe] = directed_pipes.back();
      auto [edgedir0, edge0] = directed_non_pipe_gas_edges.front();

      auto p_qvol_bar_edge0 = edge0->get_boundary_p_qvol_bar(edgedir0, state);
      double p_edge0 = p_qvol_bar_edge0[0];
      auto p_qvol_bar_lastpipe =
          lastpipe->get_boundary_p_qvol_bar(lastpipedir, state);
      double p_lastpipe = p_qvol_bar_lastpipe[0];
      int last_unused_equation_index =
          lastpipe->get_boundary_state_index(lastpipedir);

      rootvalues[last_unused_equation_index] = p_edge0 - p_lastpipe;
    }
  }

  void Bernoulligasnode::evaluate_flow_node_derivative(
      Aux::Matrixhandler *jacobianhandler,
      Eigen::Ref<Eigen::VectorXd const> const &state) const {
    if (directed_non_pipe_gas_edges.empty() and directed_pipes.empty()) {
      return;
    }

    // We will write the flow balance into the last index:
    // if non-pipes are present, take the last of them.
    // Else take the last pipe.

    int last_equation_index;
    if (!directed_non_pipe_gas_edges.empty()) {
      int last_direction = directed_non_pipe_gas_edges.back().first;
      last_equation_index =
          directed_non_pipe_gas_edges.back().second->give_away_boundary_index(
              last_direction);
    } else { // Here pipes cannot be empty!
      int last_direction = directed_pipes.back().first;
      last_equation_index =
          directed_pipes.back().second->give_away_boundary_index(
              last_direction);
    }

    // deal with pipes:
    if (!directed_pipes.empty()) {
      auto [dirpipe0, pipe0] = directed_pipes.front();
      auto p_qvol0 = pipe0->get_boundary_p_qvol_bar(dirpipe0, state);

      Eigen::RowVector2d dF_last_dpq_pipe0(0.0, dirpipe0);
      pipe0->dboundary_p_qvol_dstate(dirpipe0, jacobianhandler, dF_last_dpq_pipe0,
                                     last_equation_index, state);

      // If this was the only pipe, it bernoulli constant doesn't actually
      // factor in here! And so we go on with the non-pipe edges.
      int old_equation_index = pipe0->give_away_boundary_index(dirpipe0);
      if (directed_pipes.size() != 1) {
        Eigen::RowVector2d dF_0_dpq_pipe0 = -dbernoulli_dstate(p_qvol0, *pipe0);
        pipe0->dboundary_p_qvol_dstate(dirpipe0, jacobianhandler, dF_0_dpq_pipe0,
                                       old_equation_index, state);

        auto second_iterator = std::next(directed_pipes.begin());
        auto last_iterator = std::prev(directed_pipes.end());
        for (auto it = second_iterator; it != last_iterator; ++it) {
          auto [direction, pipe] = *it;
          int current_equation_index =
              pipe->give_away_boundary_index(direction);

          auto p_qvol_bar = pipe->get_boundary_p_qvol_bar(direction, state);
          Eigen::RowVector2d dF_old_dpq_now =
              dbernoulli_dstate(p_qvol_bar, *pipe);
          Eigen::RowVector2d dF_now_dpq_now =
              -dbernoulli_dstate(p_qvol_bar, *pipe);
          Eigen::RowVector2d dF_last_dpq_now(0.0, direction);

          pipe->dboundary_p_qvol_dstate(direction, jacobianhandler,
                                        dF_old_dpq_now, old_equation_index,
                                        state);
          pipe->dboundary_p_qvol_dstate(direction, jacobianhandler,
                                        dF_now_dpq_now, current_equation_index,
                                        state);
          pipe->dboundary_p_qvol_dstate(direction, jacobianhandler,
                                        dF_last_dpq_now, last_equation_index,
                                        state);
          old_equation_index = current_equation_index;
        }
        // last pipe:
        auto [lastpipedir, lastpipe] = directed_pipes.back();
        auto p_qvol_bar = lastpipe->get_boundary_p_qvol_bar(lastpipedir, state);
        Eigen::RowVector2d dF_old_dpq_lastpipe =
            dbernoulli_dstate(p_qvol_bar, *lastpipe);
        Eigen::RowVector2d dF_last_dpq_lastpipe(0.0, lastpipedir);
        lastpipe->dboundary_p_qvol_dstate(lastpipedir, jacobianhandler,
                                          dF_old_dpq_lastpipe,
                                          old_equation_index, state);
        lastpipe->dboundary_p_qvol_dstate(lastpipedir, jacobianhandler,
                                          dF_last_dpq_lastpipe,
                                          last_equation_index, state);
      }
    }

    if (!directed_non_pipe_gas_edges.empty()) {
      auto [dir0, edge0] = directed_non_pipe_gas_edges.front();
      Eigen::RowVector2d dF_last_dpq_0(0.0, dir0);
      edge0->dboundary_p_qvol_dstate(dir0, jacobianhandler, dF_last_dpq_0,
                                     last_equation_index, state);
      int old_equation_index = edge0->give_away_boundary_index(dir0);

      if (directed_non_pipe_gas_edges.size() != 1) {
        Eigen::RowVector2d dF_0_dpq_0(-1.0, 0.0);
        edge0->dboundary_p_qvol_dstate(dir0, jacobianhandler, dF_0_dpq_0,
                                       old_equation_index, state);

        // first and last attached edge are special:
        auto second_iterator = std::next(directed_non_pipe_gas_edges.begin());
        auto last_iterator = std::prev(directed_non_pipe_gas_edges.end());
        for (auto it = second_iterator; it != last_iterator; ++it) {
          auto [direction, edge] = *it;

          int current_equation_index =
              edge->give_away_boundary_index(direction);
          Eigen::RowVector2d dF_old_dpq_now(1.0, 0.0);
          Eigen::RowVector2d dF_now_dpq_now(-1.0, 0.0);
          Eigen::RowVector2d dF_last_dpq_now(0.0, direction);

          // Let the attached edge write out the derivative:
          edge->dboundary_p_qvol_dstate(direction, jacobianhandler,
                                        dF_old_dpq_now, old_equation_index,
                                        state);
          edge->dboundary_p_qvol_dstate(direction, jacobianhandler,
                                        dF_now_dpq_now, current_equation_index,
                                        state);
          edge->dboundary_p_qvol_dstate(direction, jacobianhandler,
                                        dF_last_dpq_now, last_equation_index,
                                        state);
          old_equation_index = current_equation_index;
        }

        // last edge:
        auto [dirlast, edgelast] = directed_non_pipe_gas_edges.back();
        Eigen::RowVector2d dF_old_dpq_last(1.0, 0.0);
        edgelast->dboundary_p_qvol_dstate(dirlast, jacobianhandler,
                                          dF_old_dpq_last, old_equation_index,
                                          state);
        Eigen::RowVector2d dF_last_dpq_last(0.0, dirlast);
        edgelast->dboundary_p_qvol_dstate(dirlast, jacobianhandler,
                                          dF_last_dpq_last, last_equation_index,
                                          state);
      }
    }
    if(!directed_pipes.empty() and !directed_non_pipe_gas_edges.empty()){
      auto [lastpipedir,lastpipe] = directed_pipes.back();
      auto [dir0, edge0] = directed_non_pipe_gas_edges.front();
      int last_unused_equation_index =
        lastpipe->get_boundary_state_index(lastpipedir);
      Eigen::RowVector2d dF_lastunused_dpq_lastpipe(-1.0,0.0);
      Eigen::RowVector2d dF_lastunused_dpq_edge0(1.0, 0.0);
      lastpipe->dboundary_p_qvol_dstate(lastpipedir,jacobianhandler, dF_lastunused_dpq_lastpipe, last_unused_equation_index,state);
      edge0->dboundary_p_qvol_dstate(dir0,jacobianhandler, dF_lastunused_dpq_edge0, last_unused_equation_index,state);
    }
  }

  void Bernoulligasnode::setup() {

    if ((!directed_non_pipe_gas_edges.empty()) or (!directed_pipes.empty())) {
      std::cout << YELLOW << "You are calling setup a second time!" << RESET
                << std::endl;
    }

    directed_non_pipe_gas_edges.clear();
    directed_pipes.clear();

    for (auto &startedge : get_starting_edges()) {
      auto startgasedge = dynamic_cast<Gasedge *>(startedge);
      if (!startgasedge) {
        std::cout << __FILE__ << ":" << __LINE__ << " Warning: The non-gasedge"
                  << startedge->get_id() << " is attached at node " << get_id()
                  << std::endl;
        continue;
      }
      auto *startpipe = dynamic_cast<Pipe *>(startgasedge);
      if (startpipe) {
        directed_pipes.push_back({1, startpipe});
      } else {
        directed_non_pipe_gas_edges.push_back({1, startgasedge});
      }
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
      auto *endpipe = dynamic_cast<Pipe *>(endgasedge);
      if (endpipe) {
        directed_pipes.push_back({-1, endpipe});
      } else {
        directed_non_pipe_gas_edges.push_back({-1, endgasedge});
      }
    }

    // Notify the user of unconnected nodes:
    if (directed_non_pipe_gas_edges.empty() and directed_pipes.empty()) {
      std::cout << "Node " << get_id() << " has no attached gas edges!"
                << std::endl;
      return;
    }
  }

  int Bernoulligasnode::get_number_of_states() const {
    return number_of_state_variables;
  }

} // namespace Model::Networkproblem::Gas
