#include "nlohmann/json.hpp"
#include <string>
#include <vector>

namespace Model::Networkproblem::Aux {

void append_to_node_json_vector(
    nlohmann::json &nodes,
    std::vector<std::pair<std::string, bool>> node_types_boundary,
    nlohmann::json const &topology, nlohmann::ordered_json const &boundary);
  
}
