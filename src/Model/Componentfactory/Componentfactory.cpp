#include <Componentfactory.hpp>

#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Gaspowerconnection.hpp"
#include "Innode.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

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
      "properties" {
        "nodes": {"type": "object", "properties": {}},
        "connections": {"type": "object", "properties": {}}
      }
    }
    )"_json;

    nlohmann::json node_schemas = topology_schema["properties"]["nodes"]["properties"];


    return topology_schema;
  }

  void add_gas_components(Componentfactory &factory) {
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Gas::Source>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Gas::Sink>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Gas::Innode>>());

    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Pipe>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Shortpipe>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Controlvalve>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Compressorstation>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Gaspowerconnection>>());
  }

  void add_power_components(Componentfactory &factory) {
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Power::Vphinode>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Power::PVnode>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Power::PQnode>>());

    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Power::Transmissionline>>());
  }

  void add_all_components(Componentfactory &factory){
    add_power_components(factory);
    add_gas_components(factory);
  }

}//namespace