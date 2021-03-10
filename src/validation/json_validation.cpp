#include <iostream>
#include <sstream>
#include <filesystem>
#include <exception>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <Aux_json.hpp>

#include "json_validation.hpp"

using nlohmann::json;
using nlohmann::json_schema::json_validator;

json load_json_file(std::string const &location);

json load_json_file(std::string const &location){
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

void validation::validate_json(json const &data) {
   validation::validate_json(data, data["$schema"].get<std::string>());
   return;
}


void validation::validate_json(json const &data, std::string const &schema_location) {
  json schema = load_json_file(schema_location);
  validation::validate_json(data, schema);
}

void validation::validate_json(json const &data, json const &schema) {

  // setup validator
  json_validator validator;
  try {
    validator.set_root_schema(schema);
  } catch (const std::exception &e) {
    std::ostringstream o;
    o << "Validation of schema failed, here is why: " << e.what() << "\n";
    throw std::runtime_error(o.str());
  }

  // validate
  try {
    validator.validate(data);
  } catch (const std::exception &e) {
    std::ostringstream o;
    o << "Validation failed, here is why: " << e.what() << "\n";
    throw std::runtime_error(o.str());
  }

  // debugging
  // std::cout << "Validated Schema \"" << schema["title"].get<std::string>()<< "\"" << std::endl;
  return; 
}

