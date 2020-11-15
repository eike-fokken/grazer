#include <Equationcomponent.hpp>
#include <Net.hpp>
#include <Networkproblem.hpp>
#include <Node.hpp>
#include <memory>
#include <utility>
#include <vector>

#include <Eigen/Sparse>

namespace Model::Networkproblem {

  /// The constructor takes an instance of Net and finds out which Edges and
  /// Nodes actually hold equations to solve
  Networkproblem::Networkproblem(std::unique_ptr<Network::Net> _network)
      : network(std::move(_network)) {
    for (Network::Node *node : network->get_nodes()) {
      if (auto equationnode = dynamic_cast<Equationnode *>(node)) {
        equationnodes.push_back(equationnode);
      }
    }

    for (Network::Edge *edge : network->get_edges()) {
      if (auto equationedge = dynamic_cast<Equationedge *>(edge)) {
        equationedges.push_back(equationedge);
      }
    }
  }

  void Networkproblem::evaluate(Eigen::VectorXd &rootfunction, double last_time,
                                double new_time,
                                Eigen::VectorXd const &last_state,
                                Eigen::VectorXd const &new_state) {
    for (Model::Networkproblem::Equationedge *eqedge : equationedges) {
      eqedge->evaluate(rootfunction, last_time, new_time, last_state,
                       new_state);
    }
    for (Model::Networkproblem::Equationnode *eqnode : equationnodes) {
      eqnode->evaluate(rootfunction, last_time, new_time, last_state,
                       new_state);
    }
  }

  void Networkproblem::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      Eigen::VectorXd const &last_state, Eigen::VectorXd const &new_state) {
    for (Model::Networkproblem::Equationedge *eqedge : equationedges) {
      eqedge->evaluate_state_derivative(jacobianhandler, last_time, new_time,
                                        last_state, new_state);
    }
    for (Model::Networkproblem::Equationnode *eqnode : equationnodes) {
      eqnode->evaluate_state_derivative(jacobianhandler, last_time, new_time,
                                        last_state, new_state);
    }
  }

  int Networkproblem::reserve_indices(int const next_free_index) {
    int free_index = next_free_index;
    for (Model::Networkproblem::Equationedge *eqedge : equationedges) {
      free_index = eqedge->set_indices(free_index);
    }
    for (Model::Networkproblem::Equationnode *eqnode : equationnodes) {
      free_index = eqnode->set_indices(free_index);
    }
    return free_index;
  }

} // namespace Model::Networkproblem
