#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>

#include "json_validation.hpp"

#include "problem_data_schema.hpp"
#include "topology_schema.hpp"

using nlohmann::json;
using nlohmann::json_schema::json_validator;


inline json_validator setup_json_validator(json schema) {
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

bool validate_problem_data(json problem_data) {
  json_validator validator setup_json_validator(PROBLEM_DATA_SCHEMA);
  return validate(validator, problem_data);
}

bool validate_topology(json topology) {
  json_validator validator setup_json_validator(TOPOLOGY_SCHEMA);
  return validate(validator, topology);
}

