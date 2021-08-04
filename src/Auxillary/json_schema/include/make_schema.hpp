#pragma once
#include <nlohmann/json.hpp>
#include <optional>

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
    json boolean();
    json boolean(std::string description);
    json boolean(std::string title, std::string description);

    json length();
  } // namespace type

  json make_list_schema_of(json const &element_schema);
  json make_list_schema_of(
      nlohmann::json const &element_schema, nlohmann::json const &array_params);

  /**
   * @brief creates a schema for boundary values
   *
   * @param num_values number of values per timestamp
   * @return nlohmann::json
   */
  json make_boundary_schema(int num_values);

  /**
   * @brief Helper to create an initial value configuration json schema for
   * a component (this is then assembled into a larger initial value json)
   *
   * @param num_interpol_points The exact number of interpolation points, if
   * known. If the optional is empty, no restriction on the number.
   * @param num_values The number of values at one interpolation point
   * @param contains_interpol_point_schemas Json schemas describing properties
   * of interpolation points which at least one of the interpolation points
   * have to satisfy. (e.g. {"minimum": 0} means that at least one of the
   * interpolation points has to be non-negative)
   * @return json
   */
  json make_initial_schema(
      std::optional<int> num_interpol_points, int num_values,
      std::vector<nlohmann::json> contains_interpol_point_schemas);

  void add_required(
      json &schema, std::string const &property_name, json const &property_def);
  void add_property(
      json &schema, std::string const &property_name, json const &property_def);
  json relax_schema(json component_schema, std::vector<std::string> keys);

  /**
   * @brief Adds default values to Json Schema
   *
   * @param properties of a schema of type "object"
   * @param defaults flat dictionary of defaults, will search all levels of
   * "schema" and whenever a key is also in the defaults will add the value as
   * default
   */
  void add_defaults(json &properties, json const &defaults);
} // namespace Aux::schema
