#include "Netfactory.hpp"
#include "Aux_json.hpp"
#include "Componentfactory.hpp"
#include "Edge.hpp"
#include "Exception.hpp"
#include "Net.hpp"
#include "Node.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Model::Networkproblem {

  void check_for_duplicates(nlohmann::json &components, std::string key) {

    std::vector<std::string> component_vector;
    for (auto const &component_class : {"nodes", "connections"}) {
      if (not components.contains(component_class)) {
        continue;
      }
      for (auto &[component_type, sub_json] :
           components[component_class].items()) {
        for (auto &component : sub_json) {
          if (not component.contains("id")) {
            gthrow({
                "The json inside of the following object",
                " does not contain an id! ",
                component.dump(1, '\t'),
                "\nThe object was inside \n\n",
                key,
                "[",
                component_class,
                "][",
                component_type,
                "]\n\n",
                "Aborting...",
            });
          }
          component_vector.push_back(component["id"].get<std::string>());
        }
      }
    }
    std::sort(component_vector.begin(), component_vector.end());
    auto first_eq_id
        = std::adjacent_find(component_vector.begin(), component_vector.end());
    if (first_eq_id != component_vector.end()) {
      gthrow({"The id ", (*first_eq_id), " appears twice in ", key, " ."});
    }
  }

  void sort_json_vectors_by_id(nlohmann::json &components, std::string key) {

    check_for_duplicates(components, key);

    for (auto const &component_class : {"nodes", "connections"}) {
      if (not components.contains(component_class)) {
        continue;
      }
      for (auto &[component_type, sub_json] :
           components[component_class].items()) {
        auto &second_json_vector_json
            = components[component_class][component_type];
        // Define < function
        auto id_compare_less
            = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
          return a["id"].get<std::string>() < b["id"].get<std::string>();
        };
        std::sort(
            second_json_vector_json.begin(), second_json_vector_json.end(),
            id_compare_less);
      }
    }
  }

  nlohmann::json
  build_full_networkproblem_json(nlohmann::json &networkproblem_json) {
    std::string topology_key = "topology_json";
    std::string boundary_key = "boundary_json";
    std::string control_key = "control_json";

    aux_json::replace_entry_with_json_from_file(
        networkproblem_json, topology_key);
    aux_json::replace_entry_with_json_from_file(
        networkproblem_json, boundary_key);
    aux_json::replace_entry_with_json_from_file(
        networkproblem_json, control_key);

    // Sorting the component vectors by ID
    nlohmann::json &topology = networkproblem_json[topology_key];
    sort_json_vectors_by_id(topology, topology_key);

    nlohmann::json &boundary = networkproblem_json[boundary_key];
    sort_json_vectors_by_id(boundary, boundary_key);

    nlohmann::json &control = networkproblem_json[control_key];
    sort_json_vectors_by_id(control, control_key);

    // build the node vector.
    insert_second_json_in_topology_json(topology, boundary, "boundary_values");
    insert_second_json_in_topology_json(topology, control, "control_values");

    return networkproblem_json[topology_key];
  }

  // std::unique_ptr<Network::Net> build_net(nlohmann::json
  // &networkproblem_json) {

  //   auto topology = build_full_networkproblem_json(networkproblem_json);
  //   auto nodes = build_node_vector(topology["nodes"]);

  //   // build the edge vector.
  //   auto edges = build_edge_vector(topology["connections"], nodes);
  //   auto network = std::make_unique<Network::Net>(std::move(nodes),
  //   std::move(edges));

  //   return network;
  // }

  std::vector<std::unique_ptr<Network::Node>> build_node_vector(
      nlohmann::json const &node_topology,
      std::map<
          std::string,
          std::unique_ptr<Componentfactory::AbstractNodeType>> const
          &nodetypemap) {

    // Here we check, whether all nodetypes defined in the topology file were
    // built. It will throw an exception, if a node type is encountered that is
    // not known.

    for (auto const &[nodetypename, ignored] : node_topology.items()) {

      auto type_itr = nodetypemap.find(nodetypename);
      if (type_itr == nodetypemap.end()) {
        gthrow(
            {"The node type ", nodetypename,
             ", given in the topology file, is unknown to grazer."});
      }
    }

    // Now we actually construct the node vector:

    std::vector<std::unique_ptr<Network::Node>> nodes;

    for (auto const &[nodetypename, nodetype] : nodetypemap) {
      if (node_topology.contains(nodetypename)) {
        for (auto node : node_topology[nodetypename]) {
          auto current_node = nodetype->make_instance(node);
          nodes.push_back(std::move(current_node));
        }
      } else {
        std::cout << "Node type " << nodetypename
                  << " not present in the topology file." << std::endl;
      }
    }

    return nodes;
  }

  std::vector<std::unique_ptr<Network::Edge>> build_edge_vector(
      nlohmann::json const &edge_topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes,
      std::map<
          std::string,
          std::unique_ptr<Componentfactory::AbstractEdgeType>> const
          &edgetypemap) {

    // Here we check, whether all edgetypes defined in the topology file were
    // built. It will throw an exception, if a edge type is encountered that
    // is not known.

    for (auto const &[edgetypename, ignored] : edge_topology.items()) {

      auto type_itr = edgetypemap.find(edgetypename);
      if (type_itr == edgetypemap.end()) {
        gthrow(
            {"The edge type ", edgetypename,
             ", given in the topology file, is unknown to grazer."});
      }
    }

    // Now we actually construct the edge vector:
    std::vector<std::unique_ptr<Network::Edge>> edges;

    for (auto const &[edgetypename, edgetype] : edgetypemap) {
      if (edge_topology.contains(edgetypename)) {
        for (auto edge : edge_topology[edgetypename]) {
          auto current_edge = edgetype->make_instance(edge, nodes);
          edges.push_back(std::move(current_edge));
        }
      } else {
        std::cout << "Edge type " << edgetypename
                  << " not present in the topology file." << std::endl;
      }
    }

    return edges;
  }

  void insert_second_json_in_topology_json(
      nlohmann::json &topology, nlohmann::json &second_json,
      std::string const &name_of_inserted_json) {
    for (auto const &component : {"nodes", "connections"}) {
      // only fire if the json contains entries of this component.
      if (not second_json.contains(component)) {
        continue;
      }
      for (auto &[key, ignored] : second_json[component].items()) {
        if (not topology[component].contains(key)) {
          std::cout << "Note: Topology json does not contain " << component
                    << " of type " << key << ", but the "
                    << name_of_inserted_json << " json does contain such "
                    << component << "." << std::endl;
          continue;
        }
        auto &second_json_vector_json = second_json[component][key];
        auto &topology_vector_json = topology[component][key];

        auto secjson_it = second_json_vector_json.begin();
        auto top_it = topology_vector_json.begin();

        // assign the additional values to the topology json.
        while (secjson_it != second_json_vector_json.end()
               and top_it != topology_vector_json.end()) {
          auto id_compare_less
              = [](nlohmann::json const &a, nlohmann::json const &b) -> bool {
            return a["id"].get<std::string>() < b["id"].get<std::string>();
          };

          if (id_compare_less(*secjson_it, *top_it)) {
            std::cout << "The component with id "
                      << (*secjson_it)["id"].get<std::string>() << '\n'
                      << " has an entry in the " << name_of_inserted_json
                      << " json but is not given in the "
                         "topology json."
                      << std::endl;
            ++secjson_it;
          } else if (id_compare_less(*top_it, *secjson_it)) {
            gthrow(
                {"The component with id ", (*top_it)["id"].get<std::string>(),
                 "\n is given in the topology json but no ",
                 name_of_inserted_json, " condition is provided."});

          } else {
            (*top_it)[name_of_inserted_json] = (*secjson_it);
            ++secjson_it;
            ++top_it;
          }
        }
      }
    }
  }

  std::unique_ptr<Network::Net> build_net(
      nlohmann::json &networkproblem_json,
      Componentfactory::Componentfactory const &factory) {
    auto topology = build_full_networkproblem_json(networkproblem_json);
    auto nodes = build_node_vector(topology["nodes"], factory.node_type_map);

    // build the edge vector.
    auto edges = build_edge_vector(
        topology["connections"], nodes, factory.edge_type_map);
    auto network
        = std::make_unique<Network::Net>(std::move(nodes), std::move(edges));

    return network;
  }
} // namespace Model::Networkproblem
