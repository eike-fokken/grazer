/*
 * Grazer - network simulation tool
 *
 * Copyright 2020-2021 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	AGPL-3.0-or-later
 *
 * Licensed under the GNU Affero General Public License v3.0, found in
 * LICENSE.txt and at https://www.gnu.org/licenses/agpl-3.0.html
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */

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
    class prettyhandler : public nlohmann::json_schema::error_handler {
      void error(
          const json::json_pointer &ptr, const json &instance,
          const std::string &message) override {
        throw std::invalid_argument(
            std::string("At ") + ptr.to_string() + " of "
            + instance.dump(1, '\t') + " - " + message + "\n");
      }
    } handler;
    validator.validate(data, handler);
  } catch (const std::exception &e) {
    std::ostringstream o;
    std::string helper;
    if (data.contains("id")) {
      o << "The json of the following object with id "
        << data["id"].get<std::string>()
        << " does not conform to its schema!\n\n\n";
    } else {
      o << "The json of the following object json does not "
           "conform to its schema.\n"
           "It also doesn't have an entry named 'id'.\n";
    }
    o << e.what() << "\n";
    throw std::runtime_error(o.str());
  }

  // debugging
  // std::cout << "Validated Schema \"" << schema["title"].get<std::string>()<<
  // "\"" << std::endl;
  return;
}
