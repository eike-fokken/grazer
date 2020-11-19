#include <Edge.hpp>
#include <Equationcomponent.hpp>
#include <Exception.hpp>
#include <Net.hpp>
#include <Networkproblem.hpp>
#include <Node.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Sparse>

namespace Model::Networkproblem {

  /// The constructor takes an instance of Net and finds out which Edges and
  /// Nodes actually hold equations to solve
  Networkproblem::Networkproblem(std::unique_ptr<Network::Net> _network)
      : network(std::move(_network)) {
    for (Network::Node *node : network->get_nodes()) {
      if (auto equationcomponent = dynamic_cast<Equationcomponent *>(node)) {
        equationcomponents.push_back(equationcomponent);
      }
    }

    for (Network::Edge *edge : network->get_edges()) {
      if (auto equationcomponent = dynamic_cast<Equationcomponent *>(edge)) {
        equationcomponents.push_back(equationcomponent);
      }
    }
  }

  void Networkproblem::evaluate(Eigen::VectorXd &rootfunction, double last_time,
                                double new_time,
                                Eigen::VectorXd const &last_state,
                                Eigen::VectorXd const &new_state) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->evaluate(rootfunction, last_time, new_time, last_state,
                            new_state);
    }
  }

  void Networkproblem::evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      Eigen::VectorXd const &last_state, Eigen::VectorXd const &new_state) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->evaluate_state_derivative(jacobianhandler, last_time,
                                             new_time, last_state, new_state);
    }
  }

  void Networkproblem::display() const { network->display(); }

  void Networkproblem::get_initializer_list(
      std::vector<std::tuple<std::string, int, int>> &list) const {

    for (Equationcomponent *eqcomponent : equationcomponents) {
      auto idcomponent = dynamic_cast<Network::Idobject *>(eqcomponent);
      if (idcomponent == nullptr) {
        gthrow({"An equation component is not of type Idobject, which should "
                "never happen."});
      }
      list.push_back({idcomponent->get_id(), get_start_state_index(),
                      get_after_state_index()});
    }
  }

  int Networkproblem::reserve_indices(int const next_free_index) {
    int free_index = next_free_index;
    for (Equationcomponent *eqcomponent : equationcomponents) {
      free_index = eqcomponent->set_indices(free_index);
    }
    return free_index;
  }

} // namespace Model::Networkproblem
