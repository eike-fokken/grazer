#include <Aux_json.hpp>
#include <exception>
#include <filesystem>
#include <iostream>
#include <nlohmann/json-schema.hpp>
#include <nlohmann/json.hpp>
#include <sstream>

#include "schema_validation.hpp"

using nlohmann::json;
using nlohmann::json_schema::json_validator;

json load_json_file(std::string const &location);

json load_json_file(std::string const &location) {
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

void Aux::schema::validate_json(json const &data) {
  Aux::schema::validate_json(data, data["$schema"].get<std::string>());
  return;
}

void Aux::schema::validate_json(
    json const &data, std::string const &schema_location) {
  json schema = load_json_file(schema_location);
  Aux::schema::validate_json(data, schema);
}

void Aux::schema::validate_json(json const &data, json const &schema) {

  // setup validator
  json_validator validator;
  try {
    validator.set_root_schema(schema);
  } catch (const std::exception &e) {
    std::ostringstream o;
    o << "Couldn't validate the data, because the schema itself if faulty: "
      << e.what() << "\n";
    throw std::runtime_error(o.str());
  }

  // validate
  try {
    validator.validate(data);
  } catch (const std::exception &e) {
    std::ostringstream o;
    std::string helper;
    if (data.contains("id")) {
      o << "The json of object with id " << data["id"].get<std::string>()
        << " does not conform to its schema!\n\n\n"
        << data.dump(1, '\t') << "\n";
    } else {
      o << "The current object json does not conform to its schema.\n"
           "It also doesn't have an entry named 'id'.\n"
           "But you can find all its data in the following dump:\n";
    }
    o << e.what() << "\n";
    throw std::runtime_error(o.str());
  }

  // debugging
  // std::cout << "Validated Schema \"" << schema["title"].get<std::string>()<<
  // "\"" << std::endl;
  return;
}