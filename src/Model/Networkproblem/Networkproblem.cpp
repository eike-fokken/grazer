#include "Networkproblem.hpp"
#include "Aux_json.hpp"
#include "Edge.hpp"
#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include "Net.hpp"
#include "Networkproblem_helpers.hpp"
#include "Node.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <utility>
#include <vector>
// #include <execution>
namespace Model::Networkproblem {

  std::string Networkproblem::get_type() const {return "Network_problem";}

  Networkproblem::~Networkproblem() {}

  /// The constructor takes an instance of Net and finds out which Edges and
  /// Nodes actually hold equations to solve
  Networkproblem::Networkproblem(nlohmann::json &networkproblem_json) {

    std::string topology_key = "topology_json";
    std::string boundary_key = "boundary_json";
    std::string control_key = "control_json";

    aux_json::replace_entry_with_json_from_file(networkproblem_json,
                                                topology_key);
    aux_json::replace_entry_with_json_from_file(networkproblem_json,
                                                boundary_key);
    aux_json::replace_entry_with_json_from_file(networkproblem_json,
                                                control_key);

    nlohmann::json &topology = networkproblem_json[topology_key];
    nlohmann::json &boundary = networkproblem_json[boundary_key];
    nlohmann::json &control = networkproblem_json[control_key];

    // build the node vector.
    insert_second_json_in_topology_json(topology, boundary, "boundary_values");
    insert_second_json_in_topology_json(topology, control, "control_values");
    supply_overall_values_to_components(networkproblem_json);
    auto nodes = build_node_vector(topology["nodes"]);

    // build the edge vector.
    auto edges = build_edge_vector(topology["connections"], nodes);

    network = std::make_unique<Network::Net>(std::move(nodes), std::move(edges));

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


      void Networkproblem::evaluate(
          Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
          double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
          Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

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
      eqcomponent->evaluate(rootvalues, last_time, new_time, last_state,
                            new_state);
    }
  }

  void Networkproblem::evaluate_state_derivative(
      ::Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &last_state, Eigen::Ref<Eigen::VectorXd const> const &new_state) const {

    // // This should evaluate in parallel, test on bigger problems later on:
    // std::for_each(std::execution::par_unseq, equationcomponents.begin(),
    //               equationcomponents.end(), [&](auto &&eqcomponent) {
    //                 eqcomponent->evaluate_state_derivative(
    //                     jacobianhandler, last_time, new_time, last_state,
    //                     new_state);
    //               });

    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->evaluate_state_derivative(jacobianhandler, last_time,
                                             new_time, last_state, new_state);
    }
  }

  void Networkproblem::save_values(double time, Eigen::Ref<Eigen::VectorXd>new_state) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->save_values(time, new_state);
    }
  }

  void Networkproblem::print_to_files(
      std::filesystem::path const &output_directory) {
    for (Model::Networkproblem::Equationcomponent *eqcomponent :
         equationcomponents) {
      eqcomponent->print_to_files(output_directory);
    }
  }

  void Networkproblem::display() const { }

  void Networkproblem::set_initial_values(Eigen::Ref<Eigen::VectorXd>new_state,
                                          nlohmann::json initial_json) {

    nlohmann::json & initial_json_vector = initial_json["initialvalues"];
    for (Equationcomponent *eqcomponent :
                                           equationcomponents) {
      auto idcomponent = dynamic_cast<Network::Idobject *>(eqcomponent);
      if (idcomponent == nullptr) {
        gthrow({"An equation component is not of type Idobject, which should "
                "never happen."});
      }
      auto component_id = idcomponent->get_id();
      auto finder = [component_id](nlohmann::json &x) {
        auto it = x.find("id");
        return it != x.end() and it.value() == component_id;
      };
      auto initjson =
          std::find_if(initial_json_vector.begin(), initial_json_vector.end(), finder);
          if(initjson == initial_json_vector.end()) {
        std::cout << " object " << component_id << " has no initial condition." << std::endl;
      } else {
        eqcomponent->set_initial_values(new_state, *initjson);
      }
    }
  }

  int Networkproblem::reserve_indices(int const next_free_index) {
    int free_index = next_free_index;
    for (Equationcomponent *eqcomponent : equationcomponents) {
      free_index = eqcomponent->set_indices(free_index);
    }
    for (Equationcomponent *eqcomponent : equationcomponents) {
      eqcomponent->setup();
    }

    return free_index;
  }

} // namespace Model::Networkproblem
