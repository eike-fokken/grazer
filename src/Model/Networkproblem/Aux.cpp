#include <Exception.hpp>
#include <Aux.hpp>

void append_to_node_json_vector(nlohmann::json &nodes, std::vector<std::pair<std::string,bool>> node_types_boundary,
      nlohmann::json const &topology, nlohmann::ordered_json const &boundary) {

    for (auto const &[nodetype, needs_boundary_values]: node_types_boundary) {

      if (topology["nodes"].find(nodetype) != topology["nodes"].end()) {
        for (auto node : topology["nodes"][nodetype]) {
          
          if(needs_boundary_values){
            std::string nodeid = node["id"];
            auto finder = [nodeid](nlohmann::json &x) {
              auto it = x.find("id");
              return it != x.end() and it.value() == nodeid;
            };
            auto bdjson =
              std::find_if(boundary["boundarycondition"].begin(),
                           boundary["boundarycondition"].end(), finder);
            if (bdjson == boundary["boundarycondition"].end()){
              gthrow({"Network component ", nodeid, " is of type ", nodetype, " but none is provided!"});}
              node["boundary_condition"] = *bdjson;
          }
          node["type"] = nodetype;
          nodes.push_back(node);
        }
      }
    }
  }
