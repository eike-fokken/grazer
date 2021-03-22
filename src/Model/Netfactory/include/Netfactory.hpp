#pragma once
#include <nlohmann/json.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace Network{
  class Node;
  class Edge;
  class Net;
}

namespace Model::Componentfactory {
  class Componentfactory;
  class AbstractNodeType;
  class AbstractEdgeType;
}


  namespace Model::Networkproblem {

    /// \brief Constructs the full networkproblem json from file paths for the
    /// respective files and fills all component jsons with their boundary and
    /// control data.
    ///
    /// This function returns a json object which contains individual component
    /// jsons that contain all information neccessary for constructing the
    /// components.
    /// @param networkproblem_json A json describing the networkproblem.
    /// Contains topology, boundary and control data, possibly in form of file
    /// paths to the respective json files.
    /// @returns the topology json enriched with control and boundary data.

    nlohmann::json
    build_full_networkproblem_json(nlohmann::json &networkproblem_json);

    /// \brief construct Nodes of types given by the json input and put their
    /// pointers in a vector
    ///
    /// This helper function constructs Nodes of specific (final) types.
    /// @return a vector of unique pointers to Node to the newly constructed
    /// nodes.
    std::vector<std::unique_ptr<Network::Node>> build_node_vector(
        nlohmann::json const &node_topology,
        std::map<std::string, std::unique_ptr<Componentfactory::AbstractNodeType>> const &nodetypemap);

    /// \brief construct Edges of types given by the json input and put their
    /// pointers in a vector
    ///
    /// This helper function constructs Edges of specific (final) types.
    /// @return a vector of unique pointers to Edge to the newly constructed
    /// edges.
    std::vector<std::unique_ptr<Network::Edge>> build_edge_vector(
        nlohmann::json const &edge_topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes,
        std::map<std::string, std::unique_ptr<Componentfactory::AbstractEdgeType>> const &edgetypemap);

    /// \brief Enters entries from a second json object into the topology json.
    ///
    /// Used for boundary and control jsons.
    void insert_second_json_in_topology_json(
        nlohmann::json &topology, nlohmann::json &boundary,
        std::string const &name_of_inserted_json);

    /// \brief Reads values that are meant for many components and writes them
    /// into each component json.
    void supply_overall_values_to_components(nlohmann::json &network_json);

    /// \brief Constructs a \ref Network::Net "Net" object from the given json.
    ///
    /// @tparam Componentfactory Chooses the \ref
    /// Model::Componentfactory::Componentfactory_interface "Componentfactory"
    /// class and with that the types of components that are recognized.
    /// @param network_json A json containing complete jsons to all components
    /// that shall be constructed for the net.
    /// @return A unique pointer to an instance of \ref Network::Net "Net"
    /// containing all nodes and edges declared in networkproblem_json.
    /// @throw std::runtime_error if a component type is found in the topology
    /// that is not constructible in the Componentfactory.
    /// @throw std::runtime_error if the passed json has errors.

    std::unique_ptr<Network::Net> build_net(nlohmann::json &networkproblem_json, Componentfactory::Componentfactory const & factory);

  } // namespace Model::Networkproblem
