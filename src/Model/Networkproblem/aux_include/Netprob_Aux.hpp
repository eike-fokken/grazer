#include "nlohmann/json.hpp"
#include <memory>
#include <string>
#include <vector>


namespace Network {
  class Node;
  class Edge;
}

namespace Model::Networkproblem::Netprob_Aux {

  /// \brief construct Nodes of types given by the json input and put their pointers in a vector
  ///
  /// This helper function constructs Nodes of specific (final) types.
  /// @return a vector of unique pointers to Node to the newly constructed nodes.
  std::vector<std::unique_ptr<Network::Node>>
  build_node_vector(nlohmann::json const &topology,
                    nlohmann::json const &boundary);

  std::vector<std::unique_ptr<Network::Edge>>
  build_edge_vector(nlohmann::json const &topology,
                    nlohmann::json const &boundary);
} // namespace Model::Networkproblem::Netprob_Aux
