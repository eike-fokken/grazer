#include "Pressureboundarynode.hpp"
#include "Coloroutput.hpp"
#include "Edge.hpp"
#include "Exception.hpp"
#include "make_schema.hpp"
#include <Eigen/Sparse>
#include <iostream>

namespace Model::Networkproblem::Gas {

  std::optional<nlohmann::json> Pressureboundarynode::get_boundary_schema() {
    return Aux::schema::make_boundary_schema(1);
  }

  Pressureboundarynode::Pressureboundarynode(nlohmann::json const &data) :
      Gasnode(data), boundaryvalue(data["boundary_values"]) {}

  void Pressureboundarynode::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double, double new_time,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {

    if (directed_attached_gas_edges.empty()) {
      return;
    }

    auto [dir0, edge0] = directed_attached_gas_edges.front();
    auto p_qvol0 = edge0->get_boundary_p_qvol_bar(dir0, new_state);
    auto p0 = p_qvol0[0];
    int equation_index = edge0->give_away_boundary_index(dir0);
    rootvalues[equation_index] = p0 - boundaryvalue(new_time)[0];
  }

  void Pressureboundarynode::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double, double,
      Eigen::Ref<Eigen::VectorXd const>,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {
    auto [dir0, edge0] = directed_attached_gas_edges.front();
    int equation_index = edge0->give_away_boundary_index(dir0);
    Eigen::RowVector2d dF_edge_dpq_0(1.0, 0.0);
    edge0->dboundary_p_qvol_dstate(
        dir0, jacobianhandler, dF_edge_dpq_0, equation_index, new_state);
  }

  void Pressureboundarynode::setup() {

    if (directed_attached_gas_edges.size() != 0) {
      std::cout << YELLOW << "You are calling setup a second time!" << RESET
                << std::endl;
    }
    directed_attached_gas_edges.clear();

    if (get_starting_edges().size() + get_ending_edges().size() != 1) {
      gthrow(
          {"A pressure boundary node can only be attached to a single "
           "connection!"});
    }
    for (auto &startedge : get_starting_edges()) {
      auto startgasedge = dynamic_cast<Gasedge *>(startedge);
      if (!startgasedge) {
        std::cout << __FILE__ << ":" << __LINE__ << " Warning: The non-gasedge"
                  << startedge->get_id() << " is attached at node " << get_id()
                  << std::endl;
        std::cout << "node id: " << get_id() << std::endl;
        continue;
      }
      directed_attached_gas_edges.push_back({1, startgasedge});
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
      directed_attached_gas_edges.push_back({-1, endgasedge});
    }

    // Notify the user of unconnected nodes:
    if (directed_attached_gas_edges.size() != 1) {
      gthrow(
          {"A pressure boundary node can only be attached to a single "
           "connection and it must be a gas edge!"});
    }
  }

} // namespace Model::Networkproblem::Gas
