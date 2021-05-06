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

  nlohmann::json Componentfactory::get_topology_schema() {
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
      auto component_schema = Aux::schema::relax_schema(
          component->get_schema(),
          {"boundary_values", "control_values", "desired_delta_x",
           "number_of_stochastic_steps", "theta_P", "sigma_P", "theta_Q",
           "sigma_Q"});
      node_schemas[name] = Aux::schema::make_list_schema_of(component_schema);
    }
    auto &edge_schemas
        = topology_schema["properties"]["connections"]["properties"];
    for (auto const &[name, component] : this->edge_type_map) {
      auto component_schema = Aux::schema::relax_schema(
          component->get_schema(),
          {"boundary_values", "control_values", "desired_delta_x",
           "number_of_stochastic_steps", "theta_P", "sigma_P", "theta_Q",
           "sigma_Q"});
      edge_schemas[name] = Aux::schema::make_list_schema_of(component_schema);
    }

    return topology_schema;
  }

} // namespace Model::Componentfactory
