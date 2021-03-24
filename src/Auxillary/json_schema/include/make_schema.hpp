#pragma once
#include <nlohmann/json.hpp>

namespace Aux::schema {
  using nlohmann::json;
  namespace type {
    json simple(std::string type);
    json simple(std::string type, std::string description);
    json simple(std::string type, std::string title, std::string description);
    json number();
    json number(std::string description);
    json number(std::string title, std::string description);
    json string();
    json string(std::string description);
    json string(std::string title, std::string description);
    json length();
  } // namespace type

  json make_list_schema_of(json const &element_schema);
  json
  make_list_schema_of(json const &element_schema, int min_items, int max_items);

  /**
   * @brief creates a schema for boundary values
   *
   * @param num_values number of values per timestamp
   * @return nlohmann::json
   */
  json make_boundary_schema(int num_values);

  void add_required(
      json &schema, std::string const &property_name, json const &property_def);
  void add_property(
      json &schema, std::string const &property_name, json const &property_def);
} // namespace Aux::schema
