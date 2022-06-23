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
#include <Aux_json.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <schema_key_insertion.hpp>

namespace Aux::schema {
  using nlohmann::ordered_json;
  int insert_schema_key_into_data_jsons(path const &grazer_directory) {
    auto schemas = grazer_directory / "schemas";
    auto problem_dir = grazer_directory / "problem";

    if (not std::filesystem::is_directory(problem_dir)) {
      if (std::filesystem::exists(problem_dir)) {
        throw std::runtime_error(
            "Problem directory is not a directory but some other file. "
            "Aborting!");
      }
      std::filesystem::create_directory(problem_dir);
    }

    std::vector<std::string> types
        = {"boundary", "control", "initial", "topology", "problem_data"};
    for (std::string const &type : types) {
      if (std::filesystem::exists(schemas / (type + "_schema.json"))) {
        path file = problem_dir / (type + ".json");
        ordered_json old_json;
        if (std::filesystem::exists(file)) {
          old_json = aux_json::get_ordered_json_from_file_path(file);
        } else {
          old_json = {};
        }
        ordered_json result;
        result["$schema"] = "../schemas/" + type + "_schema.json";
        for (auto &[key, value] : old_json.items()) {
          result[key] = std::move(value);
        }
        aux_json::overwrite_json(file, result);
      } else {
        std::cout << "Mising schema for: " << type << ", skipping!"
                  << std::endl;
      }
    }
    return 0;
  }
} // namespace Aux::schema
