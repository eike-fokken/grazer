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

namespace Aux::schema {
  /**
   * @brief assumes that the json data has a "$schema" key which points to
   * a valid json schema to validate the data against
   *
   * @param data json to validate
   */
  void validate_json(nlohmann::json const &data);

  /**
   * @brief validates the json data against the json schema found at
   * schema_location
   *
   * @param data json to validate
   * @param schema_location path to the json schema to validate against
   *
   */
  void
  validate_json(nlohmann::json const &data, std::string const &schema_location);

  /**
   * @brief validates the json data against the json schema
   *
   * @param data json data
   * @param schema json schema
   */
  void validate_json(nlohmann::json const &data, nlohmann::json const &schema);

  /**
   * @brief Inserts the defaults from the schema into the data when the keys
   * are missing
   *
   * @param data json data
   * @param schema json schema
   */
  void apply_defaults(nlohmann::json &data, nlohmann::json const &schema);

} // namespace Aux::schema
