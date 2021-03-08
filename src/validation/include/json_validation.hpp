#pragma once
#include <aux
#include <nlohmann/json.hpp>

namespace validation {
  /**
   * @brief assumes that the json data has a "$schema" key which points to
   * a valid json schema to validate the data against
   * 
   * @param data json to validate
   * @return true 
   * @return false 
   */
  bool validate_json(nlohmann::json const &data);

  /**
   * @brief validates the json data against the json schema found at schema_location
   * 
   * @param data json to validate
   * @param schema_location path to the json schema to validate against
   * @return true if validation succeeded
   * @return false (should never happen as the function will throw an error)
   * 
   */
  bool validate_json(nlohmann::json const &data, std::string const &schema_location);
}
