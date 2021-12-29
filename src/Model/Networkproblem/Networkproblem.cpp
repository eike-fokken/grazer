#include "Networkproblem.hpp"
#include "ComponentJsonHelpers.hpp"
#include "Edge.hpp"
#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include "Idobject.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include <Eigen/Sparse>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <iterator>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
// #include <execution>
namespace Model {

  /** \brief Creates a vector of Idobject pointers from some component type.
   */
  template <typename Componenttype>
  static std::vector<std::string>
  get_ids_of_objects(std::vector<Componenttype *> const &components) {

    std::vector<std::string> ids;
    ids.reserve(components.size());
    for (auto *component : components) {
      auto idcomponent = dynamic_cast<Network::Idobject *>(component);
      if (idcomponent == nullptr) {
        gthrow(
            {"Some component stored in Networkproblem is not of type Idobject, "
             "which should never happen."});
      }
      ids.push_back(idcomponent->get_id());
    }
    return ids;
  }

  template <typename Componenttype>
  static void check_components_in_json(
      std::vector<Componenttype *> const &components, nlohmann::json json,
      std::string const &key) {

    auto ids = get_ids_of_objects(components);
    std::sort(ids.begin(), ids.end());
    auto json_ids = Aux::get_sorted_ids_of_json(json, key);

    size_t json_index = 0;
    size_t id_index = 0;
    std::ostringstream errormessage;
    std::ostringstream warningmessage;
    while (true) {
      if (json_index >= json_ids.size()) {
        for (auto additional_id_index = id_index;
             additional_id_index < ids.size(); ++additional_id_index) {
          errormessage
              << "Component with id " << ids[additional_id_index]
              << " appears in the topology and needs an entry in the json "
              << key << " but has none there.\n";
        }
        break;
      }
      if (id_index >= ids.size()) {
        for (auto additional_json_index = json_index;
             additional_json_index < json_ids.size(); ++additional_json_index) {
          warningmessage << "Component with id "
                         << json_ids[additional_json_index]
                         << " appears in json " << key
                         << ", but not in the topology!\n";
        }
        break;
      }

      if (ids[id_index] > json_ids[json_index]) {
        warningmessage << "Component with id " << json_ids[json_index]
                       << " appears in json " << key
                       << ", but not in the topology!\n";
        ++json_index;
        continue;
      } else if (ids[id_index] < json_ids[json_index]) {
        errormessage
            << "Component with id " << ids[id_index]
            << " appears in the topology and needs an entry in the json " << key
            << " but has none there.\n";
        ++id_index;
        continue;
      } else { // ids[id_index] == json_ids[json_index]
        ++id_index;
        ++json_index;
      }
    }
    if (not warningmessage.str().empty()) {
      std::cout << warningmessage.str() << std::endl;
    }
    if (not errormessage.str().empty()) {
      gthrow(
          {"\n", errormessage.str(),
           "\nNote that these may not be the only problems with the input json "
           "files.\nTry again after fixing these."});
    }
  }

  std::string Networkproblem::get_type() { return "Network_problem"; }

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
      Eigen::Index next_free_state_index, Eigen::Index next_free_control_index,
      Eigen::Index next_free_constraint_index) {
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

    auto ids = get_ids_of_objects(statecomponents);
    check_components_in_json(statecomponents, initial_json, "initial.json");
    for (auto const &component : {"nodes", "connections"}) {
      if (not initial_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype : initial_json[component]) {
        for (auto const &componentjson : componenttype) {
          auto iterator = std::find(
              ids.begin(), ids.end(), componentjson["id"].get<std::string>());
          if (iterator != ids.end()) {
            auto index = iterator - ids.begin();
            statecomponents[static_cast<size_t>(index)]->set_initial_values(
                new_state, componentjson);
          }
        }
      }
    }
  }

  Eigen::Index Networkproblem::set_state_indices(Eigen::Index next_free_index) {
    state_startindex = next_free_index;
    for (auto *statecomponent : statecomponents) {
      next_free_index = statecomponent->set_state_indices(next_free_index);
    }
    state_afterindex = next_free_index;
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
      Eigen::Ref<Eigen::VectorXd const> const &control) {
    for (auto *equationcomponent : equationcomponents) {
      equationcomponent->prepare_timestep(last_time, new_time, last_state);
    }
    for (auto *controlcomponent : controlcomponents) {
      controlcomponent->prepare_timestep(
          last_time, new_time, last_state, control);
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
      Aux::InterpolatingVector_Base &controller,
      nlohmann::json const &control_json) const {

    auto ids = get_ids_of_objects(controlcomponents);
    check_components_in_json(controlcomponents, control_json, "control.json");
    for (auto const &component : {"nodes", "connections"}) {
      if (not control_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype : control_json[component]) {
        for (auto const &componentjson : componenttype) {

          auto iterator = std::find(
              ids.begin(), ids.end(), componentjson["id"].get<std::string>());
          if (iterator != ids.end()) {
            auto index = iterator - ids.begin();
            controlcomponents[static_cast<size_t>(index)]->set_initial_controls(
                controller, componentjson);
          }
        }
      }
    }
  }

  void Networkproblem::set_lower_bounds(
      Aux::InterpolatingVector_Base &full_lower_bound_vector,
      nlohmann::json const &lower_bound_json) const {

    auto ids = get_ids_of_objects(controlcomponents);
    check_components_in_json(
        controlcomponents, lower_bound_json, "lower_bound.json");
    for (auto const &component : {"nodes", "connections"}) {
      if (not lower_bound_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype : lower_bound_json[component]) {
        for (auto const &componentjson : componenttype) {

          auto iterator = std::find(
              ids.begin(), ids.end(), componentjson["id"].get<std::string>());
          if (iterator != ids.end()) {
            auto index = iterator - ids.begin();
            controlcomponents[static_cast<size_t>(index)]->set_lower_bounds(
                full_lower_bound_vector, componentjson);
          }
        }
      }
    }
  }
  void Networkproblem::set_upper_bounds(
      Aux::InterpolatingVector_Base &full_upper_bound_vector,
      nlohmann::json const &upper_bound_json) const {

    auto ids = get_ids_of_objects(controlcomponents);
    check_components_in_json(
        controlcomponents, upper_bound_json, "upper_bound.json");
    for (auto const &component : {"nodes", "connections"}) {
      if (not upper_bound_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype : upper_bound_json[component]) {
        for (auto const &componentjson : componenttype) {

          auto iterator = std::find(
              ids.begin(), ids.end(), componentjson["id"].get<std::string>());
          if (iterator != ids.end()) {
            auto index = iterator - ids.begin();
            controlcomponents[static_cast<size_t>(index)]->set_upper_bounds(
                full_upper_bound_vector, componentjson);
          }
        }
      }
    }
  }

  Eigen::Index
  Networkproblem::set_control_indices(Eigen::Index next_free_index) {
    control_startindex = next_free_index;
    for (auto *controlcomponent : controlcomponents) {
      next_free_index = controlcomponent->set_control_indices(next_free_index);
    }
    control_afterindex = next_free_index;
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

  double Networkproblem::evaluate_cost(
      double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    double cost = 0;
    for (auto *costcomponent : costcomponents) {
      cost += costcomponent->evaluate_cost(new_time, state, control);
    }
    return get_cost_weight() * cost;
  }

  void Networkproblem::d_evaluate_cost_d_state(
      Aux::Matrixhandler &cost_new_state_jacobian_handler, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *costcomponent : costcomponents) {
      costcomponent->d_evaluate_cost_d_state(
          cost_new_state_jacobian_handler, new_time, state, control);
    }
  }

  void Networkproblem::d_evaluate_cost_d_control(
      Aux::Matrixhandler &cost_control_jacobian_handler, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *costcomponent : costcomponents) {
      costcomponent->d_evaluate_cost_d_control(
          cost_control_jacobian_handler, new_time, state, control);
    }
  }

  /////////////////////////////////////////////////////////
  // constraint methods:
  /////////////////////////////////////////////////////////

  void Networkproblem::evaluate_constraint(
      Eigen::Ref<Eigen::VectorXd> constraint_values, double time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *constraintcomponent : constraintcomponents) {
      constraintcomponent->evaluate_constraint(
          constraint_values, time, state, control);
    }
  }

  void Networkproblem::d_evaluate_constraint_d_state(
      Aux::Matrixhandler &constraint_new_state_jacobian_handler, double time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *constraintcomponent : constraintcomponents) {
      constraintcomponent->d_evaluate_constraint_d_state(
          constraint_new_state_jacobian_handler, time, state, control);
    }
  }

  void Networkproblem::d_evaluate_constraint_d_control(
      Aux::Matrixhandler &constraint_control_jacobian_handler, double time,
      Eigen::Ref<Eigen::VectorXd const> const &state,
      Eigen::Ref<Eigen::VectorXd const> const &control) const {
    for (auto *constraintcomponent : constraintcomponents) {
      constraintcomponent->d_evaluate_constraint_d_control(
          constraint_control_jacobian_handler, time, state, control);
    }
  }

  Eigen::Index
  Networkproblem::set_constraint_indices(Eigen::Index next_free_index) {
    constraint_startindex = next_free_index;
    for (auto *constraintcomponent : constraintcomponents) {
      next_free_index
          = constraintcomponent->set_constraint_indices(next_free_index);
    }
    constraint_afterindex = next_free_index;
    return next_free_index;
  }

  void Networkproblem::set_constraint_lower_bounds(
      Aux::InterpolatingVector_Base &full_lower_bounds_vector,
      nlohmann::json const &constraint_lower_bounds_json) const {
    auto ids = get_ids_of_objects(constraintcomponents);
    check_components_in_json(
        constraintcomponents, constraint_lower_bounds_json,
        "constraint_lower_bound.json");

    for (auto const &component : {"nodes", "connections"}) {
      if (not constraint_lower_bounds_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype :
           constraint_lower_bounds_json[component]) {
        for (auto const &componentjson : componenttype) {

          auto iterator = std::find(
              ids.begin(), ids.end(), componentjson["id"].get<std::string>());
          if (iterator != ids.end()) {
            auto index = iterator - ids.begin();
            constraintcomponents[static_cast<size_t>(index)]
                ->set_constraint_lower_bounds(
                    full_lower_bounds_vector, constraint_lower_bounds_json);
          }
        }
      }
    }
  }

  void Networkproblem::set_constraint_upper_bounds(
      Aux::InterpolatingVector_Base &full_upper_bound_vector,
      nlohmann::json const &constraint_upper_bounds_json) const {

    auto ids = get_ids_of_objects(constraintcomponents);
    check_components_in_json(
        constraintcomponents, constraint_upper_bounds_json,
        "constraint_upper_bound.json");

    for (auto const &component : {"nodes", "connections"}) {
      if (not constraint_upper_bounds_json.contains(component)) {
        continue;
      }
      for (auto const &componenttype :
           constraint_upper_bounds_json[component]) {
        for (auto const &componentjson : componenttype) {

          auto iterator = std::find(
              ids.begin(), ids.end(), componentjson["id"].get<std::string>());
          if (iterator != ids.end()) {
            auto index = iterator - ids.begin();
            constraintcomponents[static_cast<size_t>(index)]
                ->set_constraint_upper_bounds(
                    full_upper_bound_vector, constraint_upper_bounds_json);
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
