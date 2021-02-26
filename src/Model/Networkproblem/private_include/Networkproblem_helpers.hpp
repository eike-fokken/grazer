#pragma once
#include <nlohmann/json.hpp>
#include <memory>
#include <string>
#include <vector>


namespace Network {
  class Node;
  class Edge;
}

namespace Model::Networkproblem {

  /// \brief construct Nodes of types given by the json input and put their pointers in a vector
  ///
  /// This helper function constructs Nodes of specific (final) types.
  /// @return a vector of unique pointers to Node to the newly constructed nodes.
  std::vector<std::unique_ptr<Network::Node>>
  build_node_vector(nlohmann::json const &topology);

  /// \brief construct Edges of types given by the json input and put their
  /// pointers in a vector
  ///
  /// This helper function constructs Edges of specific (final) types.
  /// @return a vector of unique pointers to Edge to the newly constructed
  /// edges.
  std::vector<std::unique_ptr<Network::Edge>>
  build_edge_vector(nlohmann::json const &topology,
                    std::vector<std::unique_ptr<Network::Node>> & nodes);


  /// \brief Enters entries from a second json object into the topology json.
  ///
  /// Used for boundary and control jsons.
  void
  insert_second_json_in_topology_json(nlohmann::json &topology,
                                      nlohmann::json &boundary,
                                      std::string const &name_of_inserted_json);

  /// \brief Reads values that are meant for many components and writes them
  /// into each component json.
  void supply_overall_values_to_components(nlohmann::json &network_json);

} // namespace Model::Networkproblem
