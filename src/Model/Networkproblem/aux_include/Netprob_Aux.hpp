#include "nlohmann/json.hpp"
#include <Vphinode.hpp>
#include <PVnode.hpp>
#include <PQnode.hpp>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace Model::Networkproblem::Netprob_Aux {



  /// \brief construct Nodes of types given by the json input and put their pointers in a vector
  ///
  /// This helper function constructs Nodes of specific (final) types.
  /// @return a vector of unique pointers to Node to the newly constructed nodes.
  std::vector<std::unique_ptr<Network::Node>> build_node_vector(nlohmann::json const &topology, nlohmann::ordered_json const &boundary);

  std::unique_ptr<Network::Node>
  construct_node(std::string const &nodetype, nlohmann::json const &topology);

  
} // namespace Model::Networkproblem::Netprob_Aux
