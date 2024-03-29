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

  /**
   * @brief Adds default values to Json Schema
   *
   * @param schema target for the defaults
   * @param defaults json dictionary of defaults
   */
  void add_defaults(json &schema, json const &defaults);

  void remove_defaults_from_required(json &schema);
} // namespace Aux::schema
