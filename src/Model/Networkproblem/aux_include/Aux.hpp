
#include "nlohmann/json.hpp"
#include <memory>
#include <string>
#include <vector>


namespace Network {
  class Node;
}

namespace Model::Networkproblem::Aux {


  /// \brief construct Nodes of types given by the json input and put their pointers in a vector
  ///
  /// This helper function constructs Nodes of specific (final) types.
  /// @return a vector of unique pointers to Node to the newly constructed nodes.
  std::vector<std::unique_ptr<Network::Node>> build_node_vector(
      std::vector<std::pair<std::string const, bool const>> node_types_boundary,
      nlohmann::json const &topology, nlohmann::ordered_json const &boundary);

  std::unique_ptr<Network::Node>
  construct_node(std::string const &nodetype, nlohmann::json const &topology);

  extern std::vector<std::pair<std::string const, bool const>> Node_names_and_boundary_demands;


  /// \brief Typedef for Code clarity
  ///
  /// This is a function pointer for build_specific_node<>().
  using Constructorfunction =
      std::unique_ptr<Network::Node> (* const)(nlohmann::json const &);

  /// \brief A map of functions that construct a Node of a specific type
  ///
  /// The key is a identifying string of the Node type and should be the same
  /// as the json key, which describes all nodes of this type.
  /// This map is used to dispatch on the string for constructing the right
  /// node.
  extern std::map<std::string const, Constructorfunction>
      node_constructor_map;


  // this macro collection is ugly but hopefully makes for clearer compiler errors:
#define STRINGIFY(string) STRINGIFYII(string)
#define STRINGIFYII(string) #string
#define LINE_NUMBER_STRING STRINGIFY(__LINE__)


  /// \brief Calls the relevant Node constructor
  ///
  /// @returns a unique pointer cast to Node.
  template <typename Nodetype>
  std::unique_ptr<Network::Node>
  build_specific_node(nlohmann::json const &topology) {
    static_assert(std::is_base_of<Network::Node, Nodetype>::value,
                  __FILE__ ":" LINE_NUMBER_STRING
                           ": Nodetype must inherit from Network::Node!");
    return std::make_unique<Nodetype>(Nodetype(topology));
  };

} // namespace Model::Networkproblem::Aux
