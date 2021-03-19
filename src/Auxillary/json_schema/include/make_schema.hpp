#pragma once
#include <nlohmann/json.hpp>

using nlohmann::json;

namespace Aux::schema {
  namespace type {
    const json numeric = R"({"type": "numeric"})"_json;
    const json length = R"({"$ref": "unit_schema.json#/length"})"_json;
  }

  json make_list_schema_of(json const &element_schema);
  json make_list_schema_of(json const &element_schema, int min_items, int max_items);

  /**
   * @brief creates a schema for boundary values
   * 
   * @param num_values number of values per timestamp
   * @return nlohmann::json 
   */
  json make_boundary_schema(int num_values);

  void add_required(json &schema, std::string const &property_name, json const &property_def);
  void add_property(json &schema, std::string const &property_name, json const &property_def);
}