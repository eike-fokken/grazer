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
} // namespace Aux::schema
