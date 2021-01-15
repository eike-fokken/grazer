#include <PQnode.hpp>
#include <PVnode.hpp>
#include <Vphinode.hpp>
#include <Netprob_Aux.hpp>
#include <Exception.hpp>
#include <iostream>
#include <memory>
#include <Choosercontainer.hpp>

namespace Model::Networkproblem::Netprob_Aux {

  std::vector<std::unique_ptr<Network::Node>> build_node_vector(
      nlohmann::json const &node_topology, nlohmann::ordered_json const &boundary) {

    Nodechooser nodechooser;

    auto nodetypedata_collection = nodechooser.get_map();

    // Here we check, whether all nodetypes defined in the topology file were built.
    // It will throw an exception, if a node type is encountered that is not known.

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
    // Reserve the number of elements in nodes so that the nodes themselves are
    // constructed one after another in memory.
    // One should check whether this makes a difference in runtime.

    // must be checked: This is just the number of node types!
    // nodes.reserve(node_topology.size());

    for (auto const & [nodetype, nodedata]: nodetypedata_collection) {
      if (node_topology.find(nodetype) != node_topology.end()) {
        // get the right constructor for the current nodetype.
        for (auto node : node_topology[nodetype]) {
          
          // If this component needs boundary values we add them to the topology json:
          if (nodedata.needs_boundary_values) {
            auto nodeid = node["id"].get<std::string>();
            auto finder = [nodeid](nlohmann::json const &x) {
              auto it = x.find("id");
              return it != x.end() and it.value() == nodeid;
            };
            auto bdjson =
                std::find_if(boundary["boundarycondition"].begin(),
                             boundary["boundarycondition"].end(), finder);
            if (bdjson == boundary["boundarycondition"].end()) {
              gthrow({"Network component ", nodeid, " is of type ", nodetype,
                      " and hence needs a boundary condition, but none is ",
                      "provided!"});
            }
            node["boundary_values"]= *bdjson;
          }

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
    build_edge_vector(nlohmann::json const &topology,
                      nlohmann::ordered_json const &boundary) {

    }

    } // namespace Model::Networkproblem::Aux
