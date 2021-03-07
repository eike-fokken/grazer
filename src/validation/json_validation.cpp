#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

#include "json_validation.hpp"

using nlohmann::json;
using nlohmann::json_schema::json_validator;


json load_json_file(std::string location);

inline json_validator setup_json_validator(std::string schema_location) {
  json schema load_json_file(schema_location);

  json_validator validator;
  try {
    validator.set_root_schema(schema);
  } catch (const std::exception &e) {
    std::cerr << "Validation of schema failed, here is why: " << e.what() << "\n";
    throw;
  }
  return validator;
}

inline bool validate(json_validator validator, json data) {
  try {
    validator.validate(data);
  } catch (const std::exception &e) {
    std::cerr << "Validation failed, here is why: " << e.what() << "\n";
    throw;
  }
  return true;
}

bool validate_json(json data) {
  json_validator validator setup_json_validator(data["$schema"]);
  return validate(validator, data);
}
