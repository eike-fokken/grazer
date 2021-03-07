#include <iostream>
#include <filesystem>
#include <exception>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <Aux_json.hpp>

#include "json_validation.hpp"

using nlohmann::json;
using nlohmann::json_schema::json_validator;


json load_json_file(std::string location){
  if (location.rfind("http", 0) == 0) {
    // url 
    throw "Not Implemented yet";
  } else {
    auto path = std::filesystem::path(location);
    if (std::filesystem::is_regular_file(path)) {
      return aux_json::get_json_from_file_path(path);
    } else {
      throw "Neither File nor URL";
    }
  }
}

bool nlohmann::validate_json(json data) {
  return validate_json(data, data["$schema"].get<std::string>());
}

bool nlohmann::validate_json(json data, std::string schema_location) {
  json schema = load_json_file(schema_location);

  // setup validator
  json_validator validator;
  try {
    validator.set_root_schema(schema);
  } catch (const std::exception &e) {
    std::cerr << "Validation of schema failed, here is why: " << e.what() << "\n";
    throw;
  }

  // validate
  try {
    validator.validate(data);
  } catch (const std::exception &e) {
    std::cerr << "Validation failed, here is why: " << e.what() << "\n";
    throw;
  }
  return true;
}

