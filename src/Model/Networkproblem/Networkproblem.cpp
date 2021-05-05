#include "Networkproblem.hpp"
#include "Edge.hpp"
#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include "Idobject.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Statecomponent.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <iterator>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>
// #include <execution>
namespace Model::Networkproblem {

  std::string Networkproblem::get_type() const { return "Network_problem"; }

  Networkproblem::~Networkproblem() {}

  /// The constructor takes an instance of Net and finds out which Edges and
  /// Nodes actually hold equations to solve
  Networkproblem::Networkproblem(std::unique_ptr<Network::Net> _network) :
      network(std::move(_network)) {
    for (Network::Node *node : network->get_nodes()) {
      if (auto equationcomponent = dynamic_cast<Equationcomponent *>(node)) {
        equationcomponents.push_back(equationcomponent);
      }
      if (auto statecomponent = dynamic_cast<Statecomponent *>(node)) {
        statecomponents.push_back(statecomponent);
      }
    }

    for (Network::Edge *edge : network->get_edges()) {
      if (auto equationcomponent = dynamic_cast<Equationcomponent *>(edge)) {
        equationcomponents.push_back(equationcomponent);
      }
      if (auto statecomponent = dynamic_cast<Statecomponent *>(edge)) {
        statecomponents.push_back(statecomponent);
      }
    }
  }

  void Networkproblem::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {

    // // This should evaluate in parallel, test on bigger problems later on:
    // std::for_each(std::execution::par_unseq, equationcomponents.begin(),
    //               equationcomponents.end(),[&](auto&& eqcomponent)
    // {
    // eqcomponent->evaluate(rootvalues, last_time, new_time, last_state,
    //                       new_state);
    // }
    // );
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->evaluate(
          rootvalues, last_time, new_time, last_state, new_state);
    }
  }

  void Networkproblem::prepare_timestep(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> new_state) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->prepare_timestep(last_time, new_time, last_state, new_state);
    }
  }

  void Networkproblem::evaluate_state_derivative(
      ::Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> last_state,
      Eigen::Ref<Eigen::VectorXd const> new_state) const {

    // // This should evaluate in parallel, test on bigger problems later on:
    // std::for_each(std::execution::par_unseq, equationcomponents.begin(),
    //               equationcomponents.end(), [&](auto &&eqcomponent) {
    //                 eqcomponent->evaluate_state_derivative(
    //                     jacobianhandler, last_time, new_time, last_state,
    //                     new_state);
    //               });

    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->evaluate_state_derivative(
          jacobianhandler, last_time, new_time, last_state, new_state);
    }
  }

  void Networkproblem::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> state) {
    for (Model::Networkproblem::Statecomponent *statecomponent :
         statecomponents) {
      statecomponent->json_save(time, state);
    }
  }

  void Networkproblem::print_to_files(nlohmann::json &new_output) {
    for (Model::Networkproblem::Statecomponent *statecomponent :
         statecomponents) {
      statecomponent->print_to_files(new_output);
    }
  }

  void Networkproblem::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state, nlohmann::json initial_json) {

    std::vector<Network::Idobject *> idcomponents;
    idcomponents.reserve(statecomponents.size());

    for (Statecomponent *statecomponent : statecomponents) {
      auto idcomponent = dynamic_cast<Network::Idobject *>(statecomponent);
      if (idcomponent == nullptr) {
        gthrow(
            {"A state component is not of type Idobject, which should "
             "never happen."});
      }
      idcomponents.push_back(idcomponent);
    }

    for (auto const &component : {"nodes", "connections"}) {
      for (auto const &componenttype : initial_json[component]) {
        for (auto const &componentjson : componenttype) {
          auto component_id = componentjson["id"];
          auto find_id = [component_id](Network::Idobject const *x) {
            return component_id == x->get_id();
          };
          auto iterator
              = std::find_if(idcomponents.begin(), idcomponents.end(), find_id);
          if (iterator != idcomponents.end()) {
            auto index = iterator - idcomponents.begin();
            statecomponents[static_cast<unsigned long>(index)]
                ->set_initial_values(new_state, componentjson);
          } else {
            std::cout
                << "Note: Component with id " << component_id
                << "appears in the initial values but not in the topology."
                << std::endl;
          }
        }
      }
    }
  }

  Network::Net const &Networkproblem::get_network() const { return *network; }

  int Networkproblem::reserve_indices(int const next_free_index) {
    int free_index = next_free_index;
    for (Statecomponent *statecomponent : statecomponents) {
      free_index = statecomponent->set_indices(free_index);
    }
    for (Equationcomponent *equationcomponent : equationcomponents) {
      equationcomponent->setup();
    }

    return free_index;
  }

} // namespace Model::Networkproblem
