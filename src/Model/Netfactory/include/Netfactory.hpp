#pragma once
#include <nlohmann/json.hpp>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>
#include "Componentfactory.hpp"


  namespace Model::Networkproblem {

    /// \brief Constructs the full networkproblem json from file paths for the
    /// respective files and fills all component jsons with their boundary and
    /// control data.
    ///
    /// @param networkproblem_json A json containining
    ... hier weiter!

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
        std::map<std::string, Componentfactory::Nodefactory> nodetypemap);

    /// \brief construct Edges of types given by the json input and put their
    /// pointers in a vector
    ///
    /// This helper function constructs Edges of specific (final) types.
    /// @return a vector of unique pointers to Edge to the newly constructed
    /// edges.
    std::vector<std::unique_ptr<Network::Edge>> build_edge_vector(
        nlohmann::json const &edge_topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes,
        std::map<std::string, Componentfactory::Edgefactory> edgetypemap);

    /// \brief Enters entries from a second json object into the topology json.
    ///
    /// Used for boundary and control jsons.
    void insert_second_json_in_topology_json(
        nlohmann::json &topology, nlohmann::json &boundary,
        std::string const &name_of_inserted_json);

    /// \brief Reads values that are meant for many components and writes them
    /// into each component json.
    void supply_overall_values_to_components(nlohmann::json &network_json);

    template <typename Componentfactory>
    std::unique_ptr<Network::Net>
    build_net(nlohmann::json &networkproblem_json) {
      static_assert(
                    std::is_base_of_v<
                    Model::Componentfactory::Componentfactory_interface<Componentfactory>,
                    Componentfactory>,
                    " The componentfactory used for networkproblem must inherit from "
                    "Componentfactory_interface<Componentfactory>.");

      Componentfactory factory;

      auto topology = build_full_networkproblem_json(networkproblem_json);
      auto nodes = build_node_vector(topology["nodes"], factory.get_nodetypemap());

      // build the edge vector.
      auto edges = build_edge_vector(topology["connections"], nodes,
                                     factory.get_edgetypemap());
      auto network =
        std::make_unique<Network::Net>(std::move(nodes), std::move(edges));

      return network;
    }

  } // namespace Model::Networkproblem
