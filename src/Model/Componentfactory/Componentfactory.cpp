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
#include "make_schema.hpp"
#include <Componentfactory.hpp>

namespace Model::Componentfactory {

  void
  Componentfactory::add_node_type(std::unique_ptr<AbstractNodeType> nodeType) {
    this->node_type_map.insert({nodeType->get_name(), std::move(nodeType)});
  }
  void
  Componentfactory::add_edge_type(std::unique_ptr<AbstractEdgeType> edgeType) {
    this->edge_type_map.insert({edgeType->get_name(), std::move(edgeType)});
  }

  nlohmann::json
  Componentfactory::get_initial_schema(bool allow_required_defaults) const {
    nlohmann::json initial_schema = R"(
    {
      "$schema": "http://json-schema.org/draft-07/schema",
      "title": "Initial Data",
      "description": "Description of the Networks Initial Data",
      "required": ["nodes", "connections"],
      "properties": {
        "nodes": {"type": "object", "additionalProperties": false, "properties": {}},
        "connections": {"type": "object", "additionalProperties": false, "properties": {}}
      }
    }
    )"_json;

    auto &node_schemas = initial_schema["properties"]["nodes"]["properties"];
    for (auto const &[name, component] : this->node_type_map) {
      auto optional_schema
          = component->get_initial_schema(allow_required_defaults);
      if (optional_schema.has_value()) {
        node_schemas[name]
            = Aux::schema::make_list_schema_of(optional_schema.value());
      }
    }
    auto &edge_schemas
        = initial_schema["properties"]["connections"]["properties"];
    for (auto const &[name, component] : this->edge_type_map) {
      auto optional_schema
          = component->get_initial_schema(allow_required_defaults);
      if (optional_schema.has_value()) {
        edge_schemas[name]
            = Aux::schema::make_list_schema_of(optional_schema.value());
      }
    }

    return initial_schema;
  }

  nlohmann::json
  Componentfactory::get_control_schema(bool allow_required_defaults) const {
    nlohmann::json control_schema = R"(
    {
      "$schema": "http://json-schema.org/draft-07/schema",
      "title": "Control",
      "description": "Description of the Controls of a Component",
      "required": ["nodes", "connections"],
      "properties": {
        "nodes": {"type": "object", "additionalProperties": false, "properties": {}},
        "connections": {"type": "object", "additionalProperties": false, "properties": {}}
      }
    }
    )"_json;

    auto &node_schemas = control_schema["properties"]["nodes"]["properties"];
    for (auto const &[name, component] : this->node_type_map) {
      auto optional_schema
          = component->get_control_schema(allow_required_defaults);
      if (optional_schema.has_value()) {
        node_schemas[name]
            = Aux::schema::make_list_schema_of(optional_schema.value());
      }
    }
    auto &edge_schemas
        = control_schema["properties"]["connections"]["properties"];
    for (auto const &[name, component] : this->edge_type_map) {
      auto optional_schema
          = component->get_control_schema(allow_required_defaults);
      if (optional_schema.has_value()) {
        edge_schemas[name]
            = Aux::schema::make_list_schema_of(optional_schema.value());
      }
    }

    return control_schema;
  }

  nlohmann::json
  Componentfactory::get_boundary_schema(bool allow_required_defaults) const {
    nlohmann::json boundary_schema = R"(
    {
      "$schema": "http://json-schema.org/draft-07/schema",
      "title": "Boundary",
      "description": "Description of the Networks Boundary Behaviour",
      "required": ["nodes", "connections"],
      "properties": {
        "nodes": {"type": "object", "additionalProperties": false, "properties": {}},
        "connections": {"type": "object", "additionalProperties": false, "properties": {}}
      }
    }
    )"_json;

    auto &node_schemas = boundary_schema["properties"]["nodes"]["properties"];
    for (auto const &[name, component] : this->node_type_map) {
      auto optional_schema
          = component->get_boundary_schema(allow_required_defaults);
      if (optional_schema.has_value()) {
        node_schemas[name]
            = Aux::schema::make_list_schema_of(optional_schema.value());
      }
    }
    auto &edge_schemas
        = boundary_schema["properties"]["connections"]["properties"];
    for (auto const &[name, component] : this->edge_type_map) {
      auto optional_schema
          = component->get_boundary_schema(allow_required_defaults);
      if (optional_schema.has_value()) {
        edge_schemas[name]
            = Aux::schema::make_list_schema_of(optional_schema.value());
      }
    }

    return boundary_schema;
  }

  nlohmann::json Componentfactory::get_topology_schema(
      bool allow_required_defaults, bool include_external) const {

    nlohmann::json topology_schema = R"(
    {
      "$schema": "http://json-schema.org/draft-07/schema",
      "title": "Topology",
      "description": "Description of the Network Topology",
      "required": ["nodes", "connections"],
      "properties": {
        "nodes": {"type": "object", "additionalProperties": false, "properties": {}},
        "connections": {"type": "object", "additionalProperties": false, "properties": {}}
      }
    }
    )"_json;

    auto &node_schemas = topology_schema["properties"]["nodes"]["properties"];
    for (auto const &[name, component] : this->node_type_map) {
      auto component_schema
          = component->get_schema(allow_required_defaults, include_external);
      node_schemas[name] = Aux::schema::make_list_schema_of(component_schema);
    }
    auto &edge_schemas
        = topology_schema["properties"]["connections"]["properties"];
    for (auto const &[name, component] : this->edge_type_map) {
      auto component_schema
          = component->get_schema(allow_required_defaults, include_external);
      edge_schemas[name] = Aux::schema::make_list_schema_of(component_schema);
    }

    return topology_schema;
  }

} // namespace Model::Componentfactory
