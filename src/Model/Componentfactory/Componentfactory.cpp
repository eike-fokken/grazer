#include <Componentfactory.hpp>
#include "make_schema.hpp"

namespace Model::Componentfactory {

  // std::map<std::string, std::unique_ptr<AbstractNodeType>> Componentfactory::get_node_type_map() {
  //   return node_type_map;
  // }
  // std::map<std::string, std::unique_ptr<AbstractEdgeType>> Componentfactory::get_edge_type_map() {
  //   return edge_type_map;
  // }

  void Componentfactory::add_node_type(std::unique_ptr<AbstractNodeType> nodeType) {
    this->node_type_map.insert({nodeType->get_name(), std::move(nodeType)});
  }
  void Componentfactory::add_edge_type(std::unique_ptr<AbstractEdgeType> edgeType) {
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

    auto & node_schemas = topology_schema["properties"]["nodes"]["properties"];
    for (auto const &[name, component] : this->node_type_map) {
      node_schemas[name] = Aux::schema::make_list_schema_of(component->get_schema());
    }
    auto & edge_schemas = topology_schema["properties"]["connections"]["properties"];
    for (auto const &[name, component] : this->edge_type_map) {
      edge_schemas[name] = Aux::schema::make_list_schema_of(component->get_schema());
    }

    return topology_schema;
  }




}//namespace