#include "Networkproblem.hpp"
#include "Edge.hpp"
#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include "Idobject.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include <Eigen/Sparse>
#include <cassert>
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
      if (auto controlcomponent = dynamic_cast<Controlcomponent *>(node)) {
        controlcomponents.push_back(controlcomponent);
      }
      if (auto costcomponent = dynamic_cast<Costcomponent *>(node)) {
        costcomponents.push_back(costcomponent);
      }
      if (auto inequalitycomponent
          = dynamic_cast<Inequalitycomponent *>(node)) {
        inequalitycomponents.push_back(inequalitycomponent);
      }
    }

    for (Network::Edge *edge : network->get_edges()) {
      if (auto equationcomponent = dynamic_cast<Equationcomponent *>(edge)) {
        equationcomponents.push_back(equationcomponent);
      }
      if (auto statecomponent = dynamic_cast<Statecomponent *>(edge)) {
        statecomponents.push_back(statecomponent);
      }
      if (auto controlcomponent = dynamic_cast<Controlcomponent *>(edge)) {
        controlcomponents.push_back(controlcomponent);
      }
      if (auto costcomponent = dynamic_cast<Costcomponent *>(edge)) {
        costcomponents.push_back(costcomponent);
      }
      if (auto inequalitycomponent
          = dynamic_cast<Inequalitycomponent *>(edge)) {
        inequalitycomponents.push_back(inequalitycomponent);
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////////
  // Statecomponent methods
  ////////////////////////////////////////////////////////////////////////////

  void Networkproblem::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
    for (Model::Networkproblem::Statecomponent *statecomponent :
         statecomponents) {
      statecomponent->json_save(time, state);
    }
  }

  void Networkproblem::add_results_to_json(nlohmann::json &new_output) {
    for (Model::Networkproblem::Statecomponent *statecomponent :
         statecomponents) {
      statecomponent->add_results_to_json(new_output);
    }
  }

  void Networkproblem::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) {

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
      if (not initial_json.contains(component)) {
        continue;
      }
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
            statecomponents[static_cast<size_t>(index)]->set_initial_values(
                new_state, componentjson);
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

  int Networkproblem::set_state_indices(int next_free_index) {
    for (Statecomponent *statecomponent : statecomponents) {
      next_free_index = statecomponent->set_state_indices(next_free_index);
    }
    return next_free_index;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Controlcomponent methods
  ////////////////////////////////////////////////////////////////////////////

  void Networkproblem::evaluate(
      Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {

    for (Model::Networkproblem::Equationcomponent *equationcomponent :
         equationcomponents) {
      equationcomponent->evaluate(
          rootvalues, last_time, new_time, last_state, new_state);
    }
    for (Model::Networkproblem::Controlcomponent *controlcomponent :
         controlcomponents) {
      controlcomponent->evaluate(
          rootvalues, last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::prepare_timestep(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) {
    for (Model::Networkproblem::Equationcomponent *equationcomponent :
         equationcomponents) {
      equationcomponent->prepare_timestep(
          last_time, new_time, last_state, new_state);
    }
    for (Model::Networkproblem::Controlcomponent *controlcomponent :
         controlcomponents) {
      controlcomponent->prepare_timestep(
          last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::d_evalutate_d_new_state(
      ::Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {

    for (Model::Networkproblem::Equationcomponent *equationcomponent :
         equationcomponents) {
      equationcomponent->d_evalutate_d_new_state(
          jacobianhandler, last_time, new_time, last_state, new_state);
    }
    for (Model::Networkproblem::Controlcomponent *controlcomponent :
         controlcomponents) {
      controlcomponent->d_evalutate_d_new_state(
          jacobianhandler, last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::d_evalutate_d_last_state(
      ::Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {

    for (Model::Networkproblem::Equationcomponent *equationcomponent :
         equationcomponents) {
      equationcomponent->d_evalutate_d_last_state(
          jacobianhandler, last_time, new_time, last_state, new_state);
    }
    for (Model::Networkproblem::Controlcomponent *controlcomponent :
         controlcomponents) {
      controlcomponent->d_evalutate_d_last_state(
          jacobianhandler, last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::d_evalutate_d_control(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (Model::Networkproblem::Controlcomponent *controlcomponent :
         controlcomponents) {
      controlcomponent->d_evalutate_d_control(
          jacobianhandler, last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::set_initial_controls_of_timestep(
      double time, Eigen::Ref<Eigen::VectorXd> controls,
      nlohmann::json const &control_json) {

    std::vector<Network::Idobject *> idcomponents;
    idcomponents.reserve(controlcomponents.size());

    for (Controlcomponent *controlcomponent : controlcomponents) {
      auto idcomponent = dynamic_cast<Network::Idobject *>(controlcomponent);
      if (idcomponent == nullptr) {
        gthrow(
            {"A control component is not of type Idobject, which should "
             "never happen."});
      }
      idcomponents.push_back(idcomponent);
    }

    for (auto const &component : {"nodes", "connections"}) {
      for (auto const &componenttype : control_json[component]) {
        for (auto const &componentjson : componenttype) {
          auto component_id = componentjson["id"];
          auto find_id = [component_id](Network::Idobject const *x) {
            return component_id == x->get_id();
          };
          auto iterator
              = std::find_if(idcomponents.begin(), idcomponents.end(), find_id);
          if (iterator != idcomponents.end()) {
            auto index = iterator - idcomponents.begin();
            controlcomponents[static_cast<size_t>(index)]
                ->set_initial_controls_of_timestep(
                    time, controls, componentjson);
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

  void
  Networkproblem::set_lower_bounds(Eigen::Ref<Eigen::VectorXd> lower_bounds) {
    for (Model::Networkproblem::Controlcomponent *controlcomponent :
         controlcomponents) {
      controlcomponent->set_lower_bounds(lower_bounds);
    }
  }

  void
  Networkproblem::set_upper_bounds(Eigen::Ref<Eigen::VectorXd> upper_bounds) {
    for (Model::Networkproblem::Controlcomponent *controlcomponent :
         controlcomponents) {
      controlcomponent->set_upper_bounds(upper_bounds);
    }
  }

  int Networkproblem::set_control_indices(int next_free_index) {
    for (Controlcomponent *controlcomponent : controlcomponents) {
      next_free_index = controlcomponent->set_control_indices(next_free_index);
    }
    return next_free_index;
  }

  void Networkproblem::setup() {
    for (Equationcomponent *equationcomponent : equationcomponents) {
      equationcomponent->setup();
    }
    for (Controlcomponent *controlcomponent : controlcomponents) {
      controlcomponent->setup();
    }
  }

  /////////////////////////////////////////////////////////
  // cost function methods:
  /////////////////////////////////////////////////////////

  void Networkproblem::evaluate_cost(
      Eigen::Ref<Eigen::VectorXd> cost_values, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (Model::Networkproblem::Costcomponent *costcomponent : costcomponents) {
      costcomponent->evaluate_cost(
          cost_values, last_time, new_time, state, control);
    }
  }

  void Networkproblem::d_evaluate_cost_d_state(
      Aux::Matrixhandler &cost_new_state_jacobian_handler, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (Model::Networkproblem::Costcomponent *costcomponent : costcomponents) {
      costcomponent->d_evaluate_cost_d_state(
          cost_new_state_jacobian_handler, last_time, new_time, state, control);
    }
  }

  void Networkproblem::d_evaluate_cost_d_control(
      Aux::Costgradienthandler &cost_control_jacobian_handler, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (Model::Networkproblem::Costcomponent *costcomponent : costcomponents) {
      costcomponent->d_evaluate_cost_d_control(
          cost_control_jacobian_handler, last_time, new_time, state, control);
    }
  }

  /////////////////////////////////////////////////////////
  // inequality methods:
  /////////////////////////////////////////////////////////

  void Networkproblem::evaluate_inequality(
      Eigen::Ref<Eigen::VectorXd> inequality_values, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (Model::Networkproblem::Inequalitycomponent *inequalitycomponent :
         inequalitycomponents) {
      inequalitycomponent->evaluate_inequality(
          inequality_values, last_time, new_time, state, control);
    }
  }

  void Networkproblem::d_evaluate_inequality_d_state(
      Aux::Matrixhandler &inequality_new_state_jacobian_handler,
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (Model::Networkproblem::Inequalitycomponent *inequalitycomponent :
         inequalitycomponents) {
      inequalitycomponent->d_evaluate_inequality_d_state(
          inequality_new_state_jacobian_handler, last_time, new_time, state,
          control);
    }
  }

  void Networkproblem::d_evaluate_inequality_d_control(
      Aux::Matrixhandler &inequality_control_jacobian_handler, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (Model::Networkproblem::Inequalitycomponent *inequalitycomponent :
         inequalitycomponents) {
      inequalitycomponent->d_evaluate_inequality_d_control(
          inequality_control_jacobian_handler, last_time, new_time, state,
          control);
    }
  }

  int Networkproblem::set_inequality_indices(int next_free_index) {
    for (Inequalitycomponent *inequalitycomponent : inequalitycomponents) {
      next_free_index
          = inequalitycomponent->set_inequality_indices(next_free_index);
    }
    return next_free_index;
  }

  /////////////////////////////////////////////////////////
  // other methods:
  /////////////////////////////////////////////////////////

  Network::Net const &Networkproblem::get_network() const { return *network; }

  int Networkproblem::reserve_control_indices(int next_free_index) {
    int free_index = next_free_index;
    for (Controlcomponent *controlcomponent : controlcomponents) {
      free_index = controlcomponent->set_control_indices(free_index);
      // Note that a component, that is a Controlcomponent can never be an
      // Equationcomponent (this should raise a compile error, see
      // check_class_hierarchy_properties in Componentfactory).
    }
    return free_index;
  }

} // namespace Model::Networkproblem
