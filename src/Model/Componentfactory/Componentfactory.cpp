#include "make_schema.hpp"
#include <Componentfactory.hpp>

namespace Model::Componentfactory {

  void validate_component_json(
      nlohmann::json const &topology, nlohmann::json const &schema) {
    try {
      validation::validate_json(topology, schema);
    } catch (std::exception &e) {

      std::string helper;
      if (topology.contains("id")) {
        helper = "The json of object with id "
                 + topology["id"].get<std::string>()
                 + " doesn't conform to its schema!\n\n\n" + topology.dump(4)
                 + "\n";
      } else {
        helper
            = "The current object json does not conform to its schema.\n"
              "It also doesn't have an entry named 'id'.\n"
              "But you can find all its data in the following dump:\n";
      }
      std::ostringstream o;
      o << helper << e.what() << "\n";
      throw std::runtime_error(o.str());
    }
  }

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
