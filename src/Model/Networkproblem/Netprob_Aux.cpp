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
      nlohmann::json const &topology, nlohmann::ordered_json const &boundary) {

    Nodechooserset nodechooserset;

    // Here comes a check whether all nodetypes where used.
    // It will throw if a node type is encountered that is not known.

    for (auto nodetype_itr = topology["nodes"].begin();
         nodetype_itr != topology["nodes"].end(); ++nodetype_itr) {
      std::cout << nodetype_itr.key() << " | " << nodetype_itr.value() << "\n";
      std::string nodetype = nodetype_itr.key();
      auto find_missing_nodetypes = [&nodetype](auto &nodechooser) {
        return nodetype == nodechooser->get_type();
      };
      auto chooser = std::find_if(nodechooserset.begin(), nodechooserset.end(),
                                  find_missing_nodetypes);
      if (chooser == nodechooserset.end()) {
        gthrow({"The node type ", nodetype, " is unknown to grazer."});
      }
    }


    // Now we actually construct the node vector:

    std::vector<std::unique_ptr<Network::Node>> nodes;
    // Reserve the number of elements in nodes so that the nodes themselves are
    // constructed one after another in memory.
    // One should check whether this makes a difference in runtime.

    // must be checked: This is just the number of node types!
    // nodes.reserve(topology["nodes"].size());

    for (auto const & nodechooser: nodechooserset) {
      std::string nodetype = nodechooser->get_type();
      if (topology["nodes"].find(nodetype) != topology["nodes"].end()) {

        // get the right constructor for the current nodetype.
        for (auto node : topology["nodes"][nodetype]) {

          // If this component needs boundary values we add them to the topology json:
          if (nodechooser->needs_boundary_values()) {
            auto nodeid = node["id"].get<std::string>();
            auto finder = [nodeid](nlohmann::json &x) {
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

          std::unique_ptr<Network::Node> current_node = nodechooser->build_specific_node(node);
          nodes.push_back(std::move(current_node));
        }
      } else {
        std::cout << "Node type " << nodetype
                  << " not present in the topology file.";
      }
    }



    return nodes;
  }

    } // namespace Model::Networkproblem::Aux
