#include "Networkproblem_helpers.hpp"
#include "Componentfactory.hpp"
#include "Exception.hpp"

#include <algorithm>
#include <iostream>
#include <memory>

namespace Model::Networkproblem {

  std::vector<std::unique_ptr<Network::Node>>
  build_node_vector(nlohmann::json const &node_topology) {

    Componentfactory::Nodechooser nodechooser;

    auto nodetypedata_collection = nodechooser.get_map();

    // Here we check, whether all nodetypes defined in the topology file were
    // built. It will throw an exception, if a node type is encountered that is
    // not known.

    for (auto nodetype_itr = node_topology.begin();
         nodetype_itr != node_topology.end(); ++nodetype_itr) {

      std::string nodetype = nodetype_itr.key();

      auto type_itr = nodetypedata_collection.find(nodetype);
      if (type_itr == nodetypedata_collection.end()) {
        gthrow({"The node type ", nodetype,
                ", given in the topology file, is unknown to grazer."});
      }
    }

    // Now we actually construct the node vector:

    std::vector<std::unique_ptr<Network::Node>> nodes;

    for (auto const &[nodetype, nodedata] : nodetypedata_collection) {
      if (node_topology.find(nodetype) != node_topology.end()) {

        for (auto node : node_topology[nodetype]) {
          auto current_node = nodedata.Constructor(node);
          nodes.push_back(std::move(current_node));
        }
      } else {
        std::cout << "Node type " << nodetype
                  << " not present in the topology file.";
      }
    }

    return nodes;
  }

  std::vector<std::unique_ptr<Network::Edge>>
  build_edge_vector(nlohmann::json const &edge_topology,
                    std::vector<std::unique_ptr<Network::Node>> &nodes) {

    Componentfactory::Edgechooser edgechooser;

    auto edgetypedata_collection = edgechooser.get_map();

    // Here we check, whether all edgetypes defined in the topology file were
    // built. It will throw an exception, if a edge type is encountered that
    // is not known.

    for (auto edgetype_itr = edge_topology.begin();
         edgetype_itr != edge_topology.end(); ++edgetype_itr) {

      std::string edgetype = edgetype_itr.key();

      auto type_itr = edgetypedata_collection.find(edgetype);
      if (type_itr == edgetypedata_collection.end()) {
        gthrow({"The edge type ", edgetype,
                ", given in the topology file, is unknown to grazer."});
      }
    }

    // Now we actually construct the edge vector:
    std::vector<std::unique_ptr<Network::Edge>> edges;

    for (auto const &[edgetype, edgedata] : edgetypedata_collection) {
      if (edge_topology.find(edgetype) != edge_topology.end()) {

        for (auto edge : edge_topology[edgetype]) {
          auto current_edge = edgedata.Constructor(edge, nodes);
          edges.push_back(std::move(current_edge));
        }
      } else {
        std::cout << "Edge type " << edgetype
                  << " not present in the topology file.";
      }
    }

    return edges;
  }

  void insert_boundary_conditions_in_topology_json(nlohmann::json &topology,
                                                   nlohmann::json &boundary) {
    // Up to now there are no edges that take boundary values, but there may be
    // some day.
    for (auto const &component : {"nodes", "connections"}) {

      // only fire if the boundary json contains entries of this component.
      if (!boundary.contains(component)) {
        continue;
      }
      for (auto it = boundary[component].begin();
           it != boundary[component].end(); ++it) {

        if(!topology[component].contains(it.key())){
          std::cout << "Note: Topology json does not contain "<< component << " of type " << it.key() << ", but the boundary json does contain such " << component << "." << std::endl;
          continue;
        }
        auto &boundary_vector_json = boundary[component][it.key()];
        auto &topology_vector_json = topology[component][it.key()];

        // define a < function as a lambda:
        auto id_compare_less = [](nlohmann::json const &a,
                                  nlohmann::json const &b) -> bool {
          return a["id"].get<std::string>() < b["id"].get<std::string>();
        };

        // Sorts the data in both topology and boundary json for easier
        // insertion afterwards.
        std::sort(boundary_vector_json.begin(), boundary_vector_json.end(),
                  id_compare_less);
        std::sort(topology_vector_json.begin(), topology_vector_json.end(),
                  id_compare_less);

        auto bnd_it = boundary_vector_json.begin();
        auto top_it = topology_vector_json.begin();

        // define an equals function as a lambda
        auto id_equals = [](nlohmann::json const &a,
                            nlohmann::json const &b) -> bool {
          return a["id"].get<std::string>() == b["id"].get<std::string>();
        };

        // Check for duplicate ids:
        auto bnd_first_pair =
            std::adjacent_find(boundary_vector_json.begin(),
                               boundary_vector_json.end(), id_equals);
        if (bnd_first_pair != boundary_vector_json.end()) {
          gthrow({"The id ", (*bnd_first_pair)["id"].get<std::string>(),
                  " appears twice in the boundary json."});
        }

        auto top_first_pair =
            std::adjacent_find(topology_vector_json.begin(),
                               topology_vector_json.end(), id_equals);
        if (top_first_pair != topology_vector_json.end()) {
          gthrow({"The id ", (*top_first_pair)["id"].get<std::string>(),
                  " appears twice in the topology json."});
        }

        // assign boundary values to the topology json.
        while (bnd_it != boundary_vector_json.end() and
               top_it != topology_vector_json.end()) {

          if (id_compare_less(*bnd_it, *top_it)) {
            std::cout << "The component with id "
                      << (*bnd_it)["id"].get<std::string>() << '\n'
                      << " has a boundary condition but is not given in the "
                         "topology json."
                      << std::endl;
            ++bnd_it;
          } else if (id_compare_less(*top_it, *bnd_it)) {
            gthrow({"The component with id ",
                    (*top_it)["id"].get<std::string>(),
                    "\n is given in the topology json but no boundary ",
                    "condition is provided."});

          } else {
            (*top_it)["boundary_values"] = (*bnd_it)["data"];
            ++bnd_it;
            ++top_it;
          }
        }
      }
    }
  }

} // namespace Model::Networkproblem
