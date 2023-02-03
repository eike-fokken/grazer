/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#pragma once
#include "Boundaryvaluecomponent.hpp"
#include "Controlcomponent.hpp"
#include "Edge.hpp"

#include "Equationcomponent.hpp"
#include "Node.hpp"
#include "Statecomponent.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

#include <map>
#include <memory>
#include <optional>
#include <sstream>

namespace Model::Componentfactory {

// this macro collection is ugly but hopefully makes for clearer compiler
// errors:
#define STRINGIFY(nonstring) STRINGIFYII(nonstring)
#define STRINGIFYII(nonstring) #nonstring
#define LINE_NUMBER_STRING STRINGIFY(__LINE__)

  /** \brief Makes sure that no component inherits from Equationcomponent and
   * Controlcomponent simultaneously. Otherwise causes a compiler error.
   *
   * @tparam Component The component to be checked.
   */
  template <typename Component>
  constexpr void check_class_hierarchy_properties() {
    static_assert(
        not(std::is_base_of_v<
                Equationcomponent,
                Component> and std::is_base_of_v<Controlcomponent, Component>),
        "\n\n\nYou have a Component type that inherits both from "
        "Equationcomponent and Controlcomponent! This is forbidden, as their "
        "evaluate methods share the same functionality.\n\nPLEASE ONLY "
        "INHERIT FROM EITHER ONE.\n\n\n");
  }

  /**
   * @brief Get the boundary schema of a component.
   * @return The schema
   */
  template <typename Component>
  nlohmann::json get_component_boundary_schema(
      nlohmann::json const &defaults, bool allow_required_defaults) {
    static_assert(
        std::is_base_of_v<Boundaryvaluecomponent, Component>,
        "This function can only be called with a class deriving from "
        "Boundaryvaluecomponent!");

    nlohmann::json boundary_schema = Component::get_boundary_schema();
    Aux::schema::add_defaults(
        boundary_schema, defaults.value("boundary_values", R"({})"_json));
    if (not allow_required_defaults) {
      Aux::schema::remove_defaults_from_required(boundary_schema);
    }
    return boundary_schema;
  }

  template <typename Component>
  nlohmann::json get_component_control_schema(
      nlohmann::json const &defaults, bool allow_required_defaults) {
    static_assert(
        std::is_base_of_v<Controlcomponent, Component>,
        "This function can only be called with a class deriving from "
        "Boundaryvaluecomponent!");

    nlohmann::json control_schema = Component::get_control_schema();
    Aux::schema::add_defaults(
        control_schema, defaults.value("control_values", R"({})"_json));
    if (not allow_required_defaults) {
      Aux::schema::remove_defaults_from_required(control_schema);
    }
    return control_schema;
  }

  template <typename Component>
  nlohmann::json get_component_initial_schema(
      nlohmann::json const &defaults, bool allow_required_defaults) {
    static_assert(
        std::is_base_of_v<Statecomponent, Component>,
        "This function can only be called with a class deriving from "
        "Boundaryvaluecomponent!");

    nlohmann::json initial_schema = Component::get_initial_schema();
    Aux::schema::add_defaults(
        initial_schema, defaults.value("initial_values", R"({})"_json));
    if (not allow_required_defaults) {
      Aux::schema::remove_defaults_from_required(initial_schema);
    }
    return initial_schema;
  }

  /**
   * @brief Common attributes of AbstractNodeTypes and AbstractEdgeTypes
   * (read their documentation for further details)
   *
   */
  struct AbstractComponentType {
    virtual ~AbstractComponentType(){};

    /**
     * @brief get the name of the component used inside the topology.json
     * definition
     * @return std::string
     */
    virtual std::string get_name() const = 0;

    /**
     * @brief Get the json schema describing the necessary properties of a
     * component
     *
     * @param allow_required_defaults whether or not attributes with a default
     * should be allowed in the required lists
     * @param include_external a toggle the inclusion of the boundary schema
     * into the topology schema
     * @return nlohmann::json
     */
    virtual nlohmann::json
    get_schema(bool allow_required_defaults, bool include_external) const = 0;

    virtual std::optional<nlohmann::json>
    get_boundary_schema(bool allow_required_defaults) const = 0;

    virtual std::optional<nlohmann::json>
    get_control_schema(bool allow_required_defaults) const = 0;

    virtual std::optional<nlohmann::json>
    get_initial_schema(bool allow_requred_defaults) const = 0;

    virtual nlohmann::json const &get_defaults() const = 0;
  };

  /**
   * @brief A NodeType is an instantiation of the class of a (concrete) node.
   * It allows the passing and storage of NodeTypes which are all subclasses of
   * AbstractNodeType. And the AbstractNodeType interface provides dynamic
   * access to the static methods of (concrete) nodes.
   *
   * Their main use is for dynamic configuration of the Componentfactories
   * by adding NodeTypes to their node_type_map. They also enable the creation
   * of JSON Schemas describing the data needed for the instantiation of the
   * (concrete) nodes.
   *
   * Preconfigured Componentfactories are found in Full_factory, Gas_factory
   * and Power_factory
   */
  struct AbstractNodeType : public AbstractComponentType {
    virtual ~AbstractNodeType(){};

    /**
     * @brief Create an instance of this NodeType
     *
     * @param topology the json snippet describing this Node from the
     * topology.json
     * @return std::unique_ptr<Network::Node>
     */
    virtual std::unique_ptr<Network::Node>
    make_instance(nlohmann::json &topology) const = 0;
  };

  /**
   * @brief A EdgeType is an instantiation of the class of a (concrete) edge.
   * It allows the passing and storage of EdgeTypes which are all subclasses of
   * AbstractEdgeType. And the AbstractEdgeType interface provides dynamic
   * access to the static methods of (concrete) edges.
   *
   * Their main use is for dynamic configuration of the Componentfactories
   * by adding EdgeTypes to their edge_type_map. They also enable the creation
   * of JSON Schemas describing the data needed for the instantiation of the
   * (concrete) edge.
   *
   * Preconfigured Componentfactories are found in Full_factory, Gas_factory
   * and Power_factory
   */
  struct AbstractEdgeType : public AbstractComponentType {
    virtual ~AbstractEdgeType(){};

    /**
     * @brief Create an instance of this EdgeType
     *
     * @param topology the json snippet describing this Edge from the
     * topology.json
     * @param nodes the adjacent nodes
     * @return std::unique_ptr<Network::Edge>
     */
    virtual std::unique_ptr<Network::Edge> make_instance(
        nlohmann::json &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes) const = 0;
  };

  /**
   * @brief A NodeType is an instantiation of the class of a (concrete) node.
   * It allows the passing and storage of NodeTypes which are all subclasses of
   * AbstractNodeType. And the AbstractNodeType interface provides dynamic
   * access to the static methods of (concrete) nodes.
   *
   * Their main use is for dynamic configuration of the Componentfactories
   * by adding NodeTypes to their node_type_map. They also enable the creation
   * of JSON Schemas describing the data needed for the instantiation of the
   * (concrete) nodes.
   *
   * Preconfigured Componentfactories are found in Full_factory, Gas_factory
   * and Power_factory
   *
   * @tparam ConcreteNode The type of the actual node. E.g. "Sink" in the gas
   * network or "Vphinode" in the power network.
   */
  template <typename ConcreteNode>
  struct NodeType final : public AbstractNodeType {
    static_assert(
        std::is_base_of_v<Network::Node, ConcreteNode>,
        __FILE__ ":" LINE_NUMBER_STRING
                 ": ConcreteNode must inherit from Network::Node.\n Check "
                 "whether an edge was added to the Nodechooser.");

    NodeType(nlohmann::json const &all_defaults) :
        defaults(all_defaults.value(ConcreteNode::get_type(), R"({})"_json)){};

    nlohmann::json const defaults;
    nlohmann::json const &get_defaults() const final { return defaults; }

    std::string get_name() const final { return ConcreteNode::get_type(); };

    nlohmann::json get_schema(
        bool allow_required_defaults, bool include_external) const final {
      nlohmann::json schema = ConcreteNode::get_schema();
      if (include_external) {
        if constexpr (std::is_base_of_v<Boundaryvaluecomponent, ConcreteNode>) {
          nlohmann::json boundary_schema
              = get_component_boundary_schema<ConcreteNode>(
                  defaults, allow_required_defaults);
          Aux::schema::add_required(schema, "boundary_values", boundary_schema);
        }
        // if constexpr (std::is_base_of_v<Controlcomponent, ConcreteNode>) {
        //   nlohmann::json control_schema
        //       = get_component_control_schema<ConcreteNode>(
        //           defaults, allow_required_defaults);
        //   Aux::schema::add_required(schema, "control_values",
        //   control_schema);
        // }
      }
      Aux::schema::add_defaults(schema, defaults);
      if (not allow_required_defaults) {
        Aux::schema::remove_defaults_from_required(schema);
      }
      return schema;
    };
    std::optional<nlohmann::json> get_boundary_schema(
        [[maybe_unused]] bool allow_required_defaults) const final {
      if constexpr (std::is_base_of_v<Boundaryvaluecomponent, ConcreteNode>) {
        return std::optional(get_component_boundary_schema<ConcreteNode>(
            defaults, allow_required_defaults));
      } else {
        return std::nullopt;
      }
    }

    std::optional<nlohmann::json> get_control_schema(
        [[maybe_unused]] bool allow_required_defaults) const final {
      if constexpr (std::is_base_of_v<Controlcomponent, ConcreteNode>) {
        return std::optional(get_component_control_schema<ConcreteNode>(
            defaults, allow_required_defaults));
      } else {
        return std::nullopt;
      }
    };

    std::optional<nlohmann::json> get_initial_schema(
        [[maybe_unused]] bool allow_required_defaults) const final {
      if constexpr (std::is_base_of_v<Statecomponent, ConcreteNode>) {
        nlohmann::json initial_schema
            = get_component_initial_schema<ConcreteNode>(
                defaults, allow_required_defaults);
        return std::optional(initial_schema);
      } else {
        return std::nullopt;
      }
    }

    std::unique_ptr<Network::Node>
    make_instance(nlohmann::json &topology) const final {
      nlohmann::json topology_schema = get_schema(
          /*allow_required_defaults=*/true, /*include_external=*/true);
      Aux::schema::apply_defaults(topology, topology_schema);
      // order is important (defaults can be required!)
      Aux::schema::validate_json(topology, topology_schema);

      check_class_hierarchy_properties<ConcreteNode>();
      return std::make_unique<ConcreteNode>(topology);
    };
  };

  /**
   * @brief A EdgeType is an instantiation of the class of a (concrete) edge.
   * It allows the passing and storage of EdgeTypes which are all subclasses
   of
   * AbstractEdgeType. And the AbstractEdgeType interface provides dynamic
   * access to the static methods of (concrete) edges.
   *
   * Their main use is for dynamic configuration of the Componentfactories
   * by adding EdgeTypes to their edge_type_map. They also enable the creation
   * of JSON Schemas describing the data needed for the instantiation of the
   * (concrete) edge.
   *
   * Preconfigured Componentfactories are found in Full_factory, Gas_factory
   * and Power_factory
   *
   * @tparam ConcreteEdge The type of the actual edge. E.g. "Pipe" in the gas
   * network or "Transmissionline" in the power network.

   */
  template <typename ConcreteEdge>
  struct EdgeType final : public AbstractEdgeType {
    static_assert(
        std::is_base_of_v<Network::Edge, ConcreteEdge>,
        __FILE__ ":" LINE_NUMBER_STRING
                 ": ConcreteEdge must inherit from Network::Edge.\n Check "
                 "whether a node was added to the Edgechooser.");
    EdgeType(nlohmann::json const &all_defaults) :
        defaults(all_defaults.value(ConcreteEdge::get_type(), R"({})"_json)){};

    nlohmann::json const defaults;
    nlohmann::json const &get_defaults() const final { return defaults; }

    std::string get_name() const final { return ConcreteEdge::get_type(); };

    nlohmann::json get_schema(
        bool allow_required_defaults, bool include_external) const final {
      nlohmann::json schema = ConcreteEdge::get_schema();
      if (include_external) {
        if constexpr (std::is_base_of_v<Boundaryvaluecomponent, ConcreteEdge>) {
          nlohmann::json boundary_schema
              = get_component_boundary_schema<ConcreteEdge>(
                  defaults, allow_required_defaults);
          Aux::schema::add_required(schema, "boundary_values", boundary_schema);
        }
        // if constexpr (std::is_base_of_v<Controlcomponent, ConcreteEdge>) {
        //   nlohmann::json control_schema
        //       = get_component_control_schema<ConcreteEdge>(
        //           defaults, allow_required_defaults);
        //   Aux::schema::add_required(schema, "control_values",
        //   control_schema);
        // }
      }
      Aux::schema::add_defaults(schema, defaults);
      if (not allow_required_defaults) {
        Aux::schema::remove_defaults_from_required(schema);
      }
      return schema;
    };

    std::optional<nlohmann::json> get_boundary_schema(
        [[maybe_unused]] bool allow_required_defaults) const final {
      if constexpr (std::is_base_of_v<Boundaryvaluecomponent, ConcreteEdge>) {
        return std::optional(get_component_boundary_schema<ConcreteEdge>(
            defaults, allow_required_defaults));
      } else {
        return std::nullopt;
      }
    }

    std::optional<nlohmann::json> get_control_schema(
        [[maybe_unused]] bool allow_required_defaults) const final {
      if constexpr (std::is_base_of_v<Controlcomponent, ConcreteEdge>) {
        return std::optional(get_component_control_schema<ConcreteEdge>(
            defaults, allow_required_defaults));
      } else {
        return std::nullopt;
      }
    };

    std::optional<nlohmann::json> get_initial_schema(
        [[maybe_unused]] bool allow_required_defaults) const final {
      if constexpr (std::is_base_of_v<Statecomponent, ConcreteEdge>) {
        nlohmann::json initial_schema
            = get_component_initial_schema<ConcreteEdge>(
                defaults, allow_required_defaults);
        return std::optional(initial_schema);
      } else {
        return std::nullopt;
      }
    }

    std::unique_ptr<Network::Edge> make_instance(
        nlohmann::json &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes) const final {
      nlohmann::json topology_schema = get_schema(
          /*allow_required_defaults=*/true, /*include_external=*/true);
      Aux::schema::apply_defaults(topology, topology_schema);
      // order is important (defaults can be required!)
      Aux::schema::validate_json(topology, topology_schema);
      check_class_hierarchy_properties<ConcreteEdge>();
      return std::make_unique<ConcreteEdge>(topology, nodes);
    };
  };

  struct Componentfactory {
  protected:
    ~Componentfactory() = default;

  public:
    /// @brief A map of Node types.
    ///
    /// The keys in this map are names of the node types as given in the
    /// topology json. The values are factory objects that can construct a
    /// node of the specific type.
    std::map<std::string, std::unique_ptr<AbstractNodeType>> node_type_map;

    /// @brief A map of Edge types.
    ///
    /// The keys in this map are names of the edge types as given in the
    /// topology json. The values are factory objects that can construct a
    /// edge of the specific type.
    std::map<std::string, std::unique_ptr<AbstractEdgeType>> edge_type_map;

    void add_node_type(std::unique_ptr<AbstractNodeType> nodeType);
    void add_edge_type(std::unique_ptr<AbstractEdgeType> edgeType);

    /**
     * @brief Return the Full JSON Schema for the Topplogy Description
     *
     * @param allow_required_defaults whether or not attributes with a default
     * should be allowed in the required lists
     * @param include_external Include schemas for data provided from other
     * files
     *
     * @return nlohmann::json
     */
    nlohmann::json get_topology_schema(
        bool allow_required_defaults, bool include_external) const;

    /**
     * @brief Return the Full JSON Schema for the Boundary Behaviour
     * Description
     *
     * @return nlohmann::json
     */
    nlohmann::json get_boundary_schema(bool allow_required_defaults) const;

    /**
     * @brief Return the Full JSON Schema for the Control Data Description
     *
     * @return nlohmann::json
     */
    nlohmann::json get_control_schema(bool allow_required_defaults) const;

    /**
     * @brief Return the Full JSON Schema for the Initial Data Description
     *
     * @return nlohmann::json
     */
    nlohmann::json get_initial_schema(bool allow_required_defaults) const;
  };

} // namespace Model::Componentfactory
