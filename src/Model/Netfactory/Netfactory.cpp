#include "Aux_json.hpp"
#include "Net.hpp"
#include "Node.hpp"
#include "Edge.hpp"
#include "Netfactory.hpp"
#include "Componentfactory.hpp"
#include "Exception.hpp"

#include <algorithm>
#include <iostream>
#include <memory>

namespace Model::Networkproblem {

  nlohmann::json build_full_networkproblem_json(nlohmann::json & networkproblem_json){
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


    return networkproblem_json[topology_key];
  }


  // std::unique_ptr<Network::Net> build_net(nlohmann::json &networkproblem_json) {

  //   auto topology = build_full_networkproblem_json(networkproblem_json);
  //   auto nodes = build_node_vector(topology["nodes"]);

  //   // build the edge vector.
  //   auto edges = build_edge_vector(topology["connections"], nodes);
  //   auto network = std::make_unique<Network::Net>(std::move(nodes), std::move(edges));

  //   return network;
  // }

  std::vector<std::unique_ptr<Network::Node>> build_node_vector(
      nlohmann::json const &node_topology,
      std::map<std::string, std::unique_ptr<Componentfactory::AbstractNodeType>> const &
          nodetypemap) {

    // Here we check, whether all nodetypes defined in the topology file were
    // built. It will throw an exception, if a node type is encountered that is
    // not known.

    for (auto nodetype_itr = node_topology.begin();
         nodetype_itr != node_topology.end(); ++nodetype_itr) {

      std::string nodetypename = nodetype_itr.key();

      auto type_itr = nodetypemap.find(nodetypename);
      if (type_itr == nodetypemap.end()) {
        gthrow({"The node type ", nodetypename,
                ", given in the topology file, is unknown to grazer."});
      }
    }

    // Now we actually construct the node vector:

    std::vector<std::unique_ptr<Network::Node>> nodes;

    for (auto const &[nodetypename, nodetype] : nodetypemap) {
      if (node_topology.find(nodetypename) != node_topology.end()) {

        for (auto node : node_topology[nodetypename]) {
          auto current_node = nodetype->make_instance(node);
          nodes.push_back(std::move(current_node));
        }
      } else {
        std::cout << "Node type " << nodetypename
                  << " not present in the topology file."
                  << std::endl;
      }
    }

    return nodes;
  }

  std::vector<std::unique_ptr<Network::Edge>> build_edge_vector(
      nlohmann::json const &edge_topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes,
      std::map<std::string, std::unique_ptr<Componentfactory::AbstractEdgeType>>
      const &edgetypemap) {

    // Here we check, whether all edgetypes defined in the topology file were
    // built. It will throw an exception, if a edge type is encountered that
    // is not known.

    for (auto edgetype_itr = edge_topology.begin();
         edgetype_itr != edge_topology.end(); ++edgetype_itr) {

      std::string edgetypename = edgetype_itr.key();

      auto type_itr = edgetypemap.find(edgetypename);
      if (type_itr == edgetypemap.end()) {
        gthrow({"The edge type ", edgetypename,
                ", given in the topology file, is unknown to grazer."});
      }
    }

    // Now we actually construct the edge vector:
    std::vector<std::unique_ptr<Network::Edge>> edges;

    for (auto const &[edgetypename, edgetype] : edgetypemap) {
      if (edge_topology.find(edgetypename) != edge_topology.end()) {

        for (auto edge : edge_topology[edgetypename]) {
          auto current_edge = edgetype->make_instance(edge, nodes);
          edges.push_back(std::move(current_edge));
        }
      } else {
        std::cout << "Edge type " << edgetypename
                  << " not present in the topology file."
                  << std::endl;
      }
    }

    return edges;
  }

  void insert_second_json_in_topology_json(nlohmann::json &topology,
                                                   nlohmann::json &second_json, std::string const & name_of_inserted_json) {
    for (auto const &component : {"nodes", "connections"}) {
      // only fire if the json contains entries of this component.
      if (!second_json.contains(component)) {
        continue;
      }
      for (auto it = second_json[component].begin();
           it != second_json[component].end(); ++it) {
        if(not topology[component].contains(it.key())){
          std::cout << "Note: Topology json does not contain "<< component << " of type " << it.key() << ", but the "<< name_of_inserted_json << " json does contain such " << component << "." << std::endl;
          continue;
        }
        auto &second_json_vector_json = second_json[component][it.key()];
        auto &topology_vector_json = topology[component][it.key()];

        // define a < function as a lambda:
        auto id_compare_less = [](nlohmann::json const &a,
                                  nlohmann::json const &b) -> bool {
          return a["id"].get<std::string>() < b["id"].get<std::string>();
        };

        // Sorts the data in both topology and boundary json for easier
        // insertion afterwards.
        std::sort(second_json_vector_json.begin(), second_json_vector_json.end(),
                  id_compare_less);
        std::sort(topology_vector_json.begin(), topology_vector_json.end(),
                  id_compare_less);

        auto secjson_it = second_json_vector_json.begin();
        auto top_it = topology_vector_json.begin();

        // define an equals function as a lambda
        auto id_equals = [](nlohmann::json const &a,
                            nlohmann::json const &b) -> bool {
          return a["id"].get<std::string>() == b["id"].get<std::string>();
        };

        // Check for duplicate ids:
        auto bnd_first_pair =
            std::adjacent_find(second_json_vector_json.begin(),
                               second_json_vector_json.end(), id_equals);
        if (bnd_first_pair != second_json_vector_json.end()) {
          gthrow({"The id ", (*bnd_first_pair)["id"].get<std::string>(),
              " appears twice in the", name_of_inserted_json, " json."});
        }

        auto top_first_pair =
            std::adjacent_find(topology_vector_json.begin(),
                               topology_vector_json.end(), id_equals);
        if (top_first_pair != topology_vector_json.end()) {
          gthrow({"The id ", (*top_first_pair)["id"].get<std::string>(),
                  " appears twice in the topology json."});
        }

        // assign the additional values to the topology json.
        while (secjson_it != second_json_vector_json.end() and
               top_it != topology_vector_json.end()) {

          if (id_compare_less(*secjson_it, *top_it)) {
            std::cout << "The component with id "
                      << (*secjson_it)["id"].get<std::string>() << '\n'
                      << " has an entry in the " << name_of_inserted_json
                      << " json but is not given in the "
                         "topology json."
                      << std::endl;
            ++secjson_it;
          } else if (id_compare_less(*top_it, *secjson_it)) {
            gthrow({"The component with id ",
                    (*top_it)["id"].get<std::string>(),
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

    void supply_overall_values_to_components(nlohmann::json & network_json) {

      // This list way well become longer!
      auto pde_components = {"Pipe"};
      if (network_json.contains("desired_delta_x")) {
        for (auto const &type : pde_components) {
          if (not network_json["topology_json"]["connections"].contains(type)) {
            continue;
            }
            for (auto &pipe :
                 network_json["topology_json"]["connections"][type]) {
              if (!pipe.contains("desired_delta_x")) {
                pipe["desired_delta_x"] = network_json["desired_delta_x"];
              } else {
                std::cout << "Object with id " << pipe["id"]
                          << " has its own value of desired_delta_x."
                          << std::endl;
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
