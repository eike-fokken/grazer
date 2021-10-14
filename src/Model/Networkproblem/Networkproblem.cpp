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
namespace Model {

  /** \brief Creates a vector of Idobject pointers from some component type.
   */
  template <typename Componenttype>
  static std::vector<Network::Idobject *>
  get_idobjects(std::vector<Componenttype *> const &components) {

    std::vector<Network::Idobject *> idcomponents;
    idcomponents.reserve(components.size());
    for (auto *component : components) {
      auto idcomponent = dynamic_cast<Network::Idobject *>(component);
      if (idcomponent == nullptr) {
        gthrow(
            {"Some component stored in Networkproblem is not of type Idobject, "
             "which should never happen."});
      }
      idcomponents.push_back(idcomponent);
    }
    return idcomponents;
  }

  std::string Networkproblem::get_type() const { return "Network_problem"; }

  Networkproblem::~Networkproblem() {}

  /// The constructor takes an instance of Net and finds out which Edges and
  /// Nodes actually hold equations to solve
  Networkproblem::Networkproblem(std::unique_ptr<Network::Net> _network) :
      network(std::move(_network)) {
    for (auto *node : network->get_nodes()) {
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
      if (auto constraintcomponent
          = dynamic_cast<Constraintcomponent *>(node)) {
        constraintcomponents.push_back(constraintcomponent);
      }
    }

    for (auto *edge : network->get_edges()) {
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
      if (auto constraintcomponent
          = dynamic_cast<Constraintcomponent *>(edge)) {
        constraintcomponents.push_back(constraintcomponent);
      }
    }
  }

  void Networkproblem::init(
      int next_free_state_index, int next_free_control_index,
      int next_free_constraint_index) {
    next_free_state_index = set_state_indices(next_free_state_index);
    next_free_control_index = set_control_indices(next_free_control_index);
    next_free_constraint_index
        = set_constraint_indices(next_free_constraint_index);
    setup();
  }

  ////////////////////////////////////////////////////////////////////////////
  // Statecomponent methods
  ////////////////////////////////////////////////////////////////////////////

  void Networkproblem::json_save(
      double time, Eigen::Ref<Eigen::VectorXd const> const &state) {
    for (auto *statecomponent : statecomponents) {
      statecomponent->json_save(time, state);
    }
  }

  void Networkproblem::add_results_to_json(nlohmann::json &new_output) {
    for (auto *statecomponent : statecomponents) {
      statecomponent->add_results_to_json(new_output);
    }
  }

  void Networkproblem::set_initial_values(
      Eigen::Ref<Eigen::VectorXd> new_state,
      nlohmann::json const &initial_json) {

    auto idcomponents = get_idobjects(statecomponents);

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
                << "\n";
          }
        }
      }
    }
  }

  int Networkproblem::set_state_indices(int next_free_index) {
    start_state_index = next_free_index;
    for (auto *statecomponent : statecomponents) {
      next_free_index = statecomponent->set_state_indices(next_free_index);
    }
    after_state_index = next_free_index;
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

    for (auto *equationcomponent : equationcomponents) {
      equationcomponent->evaluate(
          rootvalues, last_time, new_time, last_state, new_state);
    }
    for (auto *controlcomponent : controlcomponents) {
      controlcomponent->evaluate(
          rootvalues, last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::prepare_timestep(
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) {
    for (auto *equationcomponent : equationcomponents) {
      equationcomponent->prepare_timestep(
          last_time, new_time, last_state, new_state);
    }
    for (auto *controlcomponent : controlcomponents) {
      controlcomponent->prepare_timestep(
          last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::d_evalutate_d_new_state(
      ::Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {

    for (auto *equationcomponent : equationcomponents) {
      equationcomponent->d_evalutate_d_new_state(
          jacobianhandler, last_time, new_time, last_state, new_state);
    }
    for (auto *controlcomponent : controlcomponents) {
      controlcomponent->d_evalutate_d_new_state(
          jacobianhandler, last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::d_evalutate_d_last_state(
      ::Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {

    for (auto *equationcomponent : equationcomponents) {
      equationcomponent->d_evalutate_d_last_state(
          jacobianhandler, last_time, new_time, last_state, new_state);
    }
    for (auto *controlcomponent : controlcomponents) {
      controlcomponent->d_evalutate_d_last_state(
          jacobianhandler, last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::d_evalutate_d_control(
      Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state,
      Eigen::Ref<Eigen::VectorXd const> const &new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *controlcomponent : controlcomponents) {
      controlcomponent->d_evalutate_d_control(
          jacobianhandler, last_time, new_time, last_state, new_state, control);
    }
  }

  void Networkproblem::set_initial_controls(
      Timedata timedata, Aux::Controller &controller,
      nlohmann::json const &control_json) {

    auto idcomponents = get_idobjects(controlcomponents);

    for (auto const &component : {"nodes", "connections"}) {
      if (not control_json.contains(component)) {
        continue;
      }
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
            controlcomponents[static_cast<size_t>(index)]->set_initial_controls(
                timedata, controller, control_json);
          } else {
            std::cout << "Note: Component with id " << component_id
                      << "appears in the control initial values but not in the "
                         "topology."
                      << "\n";
          }
        }
      }
    }
  }

  void Networkproblem::set_lower_bounds(
      Timedata timedata, Eigen::Ref<Eigen::VectorXd> lower_bounds,
      nlohmann::json const &lower_bound_json) {

    auto idcomponents = get_idobjects(controlcomponents);

    for (auto const &component : {"nodes", "connections"}) {
      if (not lower_bound_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype : lower_bound_json[component]) {
        for (auto const &componentjson : componenttype) {

          auto component_id = componentjson["id"];

          auto find_id = [component_id](Network::Idobject const *x) {
            return component_id == x->get_id();
          };
          auto iterator
              = std::find_if(idcomponents.begin(), idcomponents.end(), find_id);
          if (iterator != idcomponents.end()) {
            auto index = iterator - idcomponents.begin();
            controlcomponents[static_cast<size_t>(index)]->set_lower_bounds(
                timedata, lower_bounds, lower_bound_json);
          } else {
            std::cout << "Note: Component with id " << component_id
                      << "appears in the lower bounds but not in the topology."
                      << "\n";
          }
        }
      }
    }
  }

  void Networkproblem::set_upper_bounds(
      Timedata timedata, Eigen::Ref<Eigen::VectorXd> upper_bounds,
      nlohmann::json const &upper_bound_json) {
    auto idcomponents = get_idobjects(controlcomponents);

    for (auto const &component : {"nodes", "connections"}) {
      if (not upper_bound_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype : upper_bound_json[component]) {
        for (auto const &componentjson : componenttype) {

          auto component_id = componentjson["id"];

          auto find_id = [component_id](Network::Idobject const *x) {
            return component_id == x->get_id();
          };
          auto iterator
              = std::find_if(idcomponents.begin(), idcomponents.end(), find_id);
          if (iterator != idcomponents.end()) {
            auto index = iterator - idcomponents.begin();
            controlcomponents[static_cast<size_t>(index)]->set_upper_bounds(
                timedata, upper_bounds, upper_bound_json);
          } else {
            std::cout << "Note: Component with id " << component_id
                      << "appears in the upper bounds but not in the topology."
                      << "\n";
          }
        }
      }
    }
  }

  int Networkproblem::set_control_indices(int next_free_index) {
    start_control_index = next_free_index;
    for (auto *controlcomponent : controlcomponents) {
      next_free_index = controlcomponent->set_control_indices(next_free_index);
    }
    after_control_index = next_free_index;
    return next_free_index;
  }

  void Networkproblem::setup() {
    for (auto *equationcomponent : equationcomponents) {
      equationcomponent->setup();
    }
    for (auto *controlcomponent : controlcomponents) {
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
    for (auto *costcomponent : costcomponents) {
      costcomponent->evaluate_cost(
          cost_values, last_time, new_time, state, control);
    }
  }

  void Networkproblem::d_evaluate_cost_d_state(
      Aux::Matrixhandler &cost_new_state_jacobian_handler, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *costcomponent : costcomponents) {
      costcomponent->d_evaluate_cost_d_state(
          cost_new_state_jacobian_handler, last_time, new_time, state, control);
    }
  }

  void Networkproblem::d_evaluate_cost_d_control(
      Aux::Costgradienthandler &cost_control_jacobian_handler, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *costcomponent : costcomponents) {
      costcomponent->d_evaluate_cost_d_control(
          cost_control_jacobian_handler, last_time, new_time, state, control);
    }
  }

  /////////////////////////////////////////////////////////
  // constraint methods:
  /////////////////////////////////////////////////////////

  void Networkproblem::evaluate_constraint(
      Eigen::Ref<Eigen::VectorXd> constraint_values, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *constraintcomponent : constraintcomponents) {
      constraintcomponent->evaluate_constraint(
          constraint_values, last_time, new_time, state, control);
    }
  }

  void Networkproblem::d_evaluate_constraint_d_state(
      Aux::Matrixhandler &constraint_new_state_jacobian_handler,
      double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *constraintcomponent : constraintcomponents) {
      constraintcomponent->d_evaluate_constraint_d_state(
          constraint_new_state_jacobian_handler, last_time, new_time, state,
          control);
    }
  }

  void Networkproblem::d_evaluate_constraint_d_control(
      Aux::Matrixhandler &constraint_control_jacobian_handler, double last_time,
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *constraintcomponent : constraintcomponents) {
      constraintcomponent->d_evaluate_constraint_d_control(
          constraint_control_jacobian_handler, last_time, new_time, state,
          control);
    }
  }

  int Networkproblem::set_constraint_indices(int next_free_index) {
    start_constraint_index = next_free_index;
    for (auto *constraintcomponent : constraintcomponents) {
      next_free_index
          = constraintcomponent->set_constraint_indices(next_free_index);
    }
    after_constraint_index = next_free_index;
    return next_free_index;
  }

  void Networkproblem::set_constraint_lower_bounds(
      Timedata timedata, Eigen::Ref<Eigen::VectorXd> constraint_lower_bounds,
      nlohmann::json const &constraint_lower_bound_json) {
    auto idcomponents = get_idobjects(constraintcomponents);

    for (auto const &component : {"nodes", "connections"}) {
      if (not constraint_lower_bound_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype : constraint_lower_bound_json[component]) {
        for (auto const &componentjson : componenttype) {

          auto component_id = componentjson["id"];

          auto find_id = [component_id](Network::Idobject const *x) {
            return component_id == x->get_id();
          };
          auto iterator
              = std::find_if(idcomponents.begin(), idcomponents.end(), find_id);
          if (iterator != idcomponents.end()) {
            auto index = iterator - idcomponents.begin();
            constraintcomponents[static_cast<size_t>(index)]
                ->set_constraint_lower_bounds(
                    timedata, constraint_lower_bounds,
                    constraint_lower_bound_json);
          } else {
            std::cout << "Note: Component with id " << component_id
                      << "appears in the constraint lower bounds but not in "
                         "the topology."
                      << "\n";
          }
        }
      }
    }
  }

  void Networkproblem::set_constraint_upper_bounds(
      Timedata timedata, Eigen::Ref<Eigen::VectorXd> constraint_upper_bounds,
      nlohmann::json const &constraint_upper_bound_json) {

    auto idcomponents = get_idobjects(constraintcomponents);

    for (auto const &component : {"nodes", "connections"}) {
      if (not constraint_upper_bound_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype : constraint_upper_bound_json[component]) {
        for (auto const &componentjson : componenttype) {

          auto component_id = componentjson["id"];

          auto find_id = [component_id](Network::Idobject const *x) {
            return component_id == x->get_id();
          };
          auto iterator
              = std::find_if(idcomponents.begin(), idcomponents.end(), find_id);
          if (iterator != idcomponents.end()) {
            auto index = iterator - idcomponents.begin();
            constraintcomponents[static_cast<size_t>(index)]
                ->set_constraint_upper_bounds(
                    timedata, constraint_upper_bounds,
                    constraint_upper_bound_json);
          } else {
            std::cout << "Note: Component with id " << component_id
                      << "appears in the constraint upper bounds but not in "
                         "the topology."
                      << "\n";
          }
        }
      }
    }
  }

  /////////////////////////////////////////////////////////
  // other methods:
  /////////////////////////////////////////////////////////

  Network::Net const &Networkproblem::get_network() const { return *network; }

} // namespace Model
