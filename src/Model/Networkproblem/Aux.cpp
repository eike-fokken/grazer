#include <PQnode.hpp>
#include <PVnode.hpp>
#include <Vphinode.hpp>
#include <Aux.hpp>
#include <Exception.hpp>
#include <iostream>
#include <memory>


namespace Model::Networkproblem::Aux {

  // Add power nodes:
  std::vector<std::pair<std::string, bool>> power_type_boundary(
      {{"Vphinode", true}, {"PVnode", true}, {"PQnode", true}});

  // Add Gas nodes:
  // Here pressure boundary conditions are missing:
  std::vector<std::pair<std::string, bool>>
      gas_type_boundary({{"Source", true}, {"Sink", true}, {"Innode", false}});

  std::map<std::string const, Constructorfunction> node_constructor_map = {
      {"Vphinode", build_specific_node<Power::Vphinode>},
      {"PVnode", build_specific_node<Power::PVnode>},
      {"PQnode", build_specific_node<Power::PQnode>},
  };

  std::vector<std::unique_ptr<Network::Node>> build_node_vector(
      std::vector<std::pair<std::string, bool>> node_types_boundary,
      nlohmann::json const &topology, nlohmann::ordered_json const &boundary) {

    std::vector<std::unique_ptr<Network::Node>> nodes;
    // Reserve the number of elements in nodes so that the nodes themselves are
    // constructed one after another in memory.
    // One should check whether this makes a difference in runtime.

    // must be checked: This is just the number of node types!
    // nodes.reserve(topology["nodes"].size());

    for (auto const &[nodetype, needs_boundary_values] : node_types_boundary) {
      if (topology["nodes"].find(nodetype) != topology["nodes"].end()) {

        // get the right constructor for the current nodetype.
        auto current_constructor = node_constructor_map.at(nodetype);


        for (auto node : topology["nodes"][nodetype]) {

          // If this component needs boundary values we add them to the topology json:
          if (needs_boundary_values) {
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

          std::unique_ptr<Network::Node> current_node = current_constructor(node);
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
