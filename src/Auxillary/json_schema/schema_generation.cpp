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
#include <Exception.hpp>
#include <fstream>
#include <iostream>
#include <schema_generation.hpp>

namespace Aux::schema {
  using Model::Componentfactory::Componentfactory;
  using std::filesystem::path;

  // private functions
  bool
  key_from_map_is_prefix(std::map<std::string, json> map, std::string string);

  /**
   * @brief directory validation (directory may only include known schema files)
   * @param directory the path to the directory
   * @param schemas the json schemas
   */
  void assert_only_known_schemas(
      path directory, std::map<std::string, json> schemas);

  int make_full_factory_schemas(path grazer_dir) {
    std::filesystem::path problem_data_path
        = grazer_dir / "problem" / "problem_data.json";
    nlohmann::json defaults = R"({})"_json;
    if (std::filesystem::exists(problem_data_path)) {
      nlohmann::json problem_data
          = aux_json::get_json_from_file_path(problem_data_path);
      try {
        defaults = problem_data.at("problem_data").at("defaults");
      } catch (nlohmann::json::out_of_range &e) {
        std::cout << "[Warning] The `problem_data.json` does not have the "
                     "expected structure:\n"
                  << "  problem_data.subproblems.Network_problem.defaults\n"
                  << "  " << e.what() << "\n"
                  << "Generating Schemas without defaults!" << std::endl;
      }
    } else {
      std::cout << "[Warning] The `problem_data.json` does not exist\n"
                << "Generating Schemas without defaults!" << std::endl;
    }
    Model::Componentfactory::Full_factory full_factory(defaults);
    make_schemas(full_factory, grazer_dir / "schemas");
    return 0;
  }

  inline static json get_problem_data_schema();

  void make_schemas(Componentfactory const &factory, path schema_dir) {

    std::map<std::string, json> schemas{
        {"problem_data", get_problem_data_schema()},
        {"topology",
         factory.get_topology_schema(
             /*allow_required_defaults=*/false, /*include_external=*/false)},
        {"initial",
         factory.get_initial_schema(/*allow_required_defaults=*/false)},
        {"boundary",
         factory.get_boundary_schema(/*allow_required_defaults=*/false)},
        {"control",
         factory.get_control_schema(/*allow_required_defaults=*/false)}};

    if (std::filesystem::exists(schema_dir)) {
      assert_only_known_schemas(schema_dir, schemas);
    } else {
      std::filesystem::create_directory(schema_dir);
    }

    for (auto const &[name, schema] : schemas) {
      path file = schema_dir / path(name + "_schema.json");
      std::ofstream ofs(file);
      ofs << schema.dump(
          /*indent=*/1, /*indent_char=*/'\t');
    }
  }

  void assert_only_known_schemas(
      path directory, std::map<std::string, json> schemas) {
    for (auto const &file : std::filesystem::directory_iterator(directory)) {
      std::string filename = file.path().filename().string();
      if (std::filesystem::is_directory(file)) {
        gthrow(
            {"unknown directory ", filename, " inside ", directory.string(),
             " aborting to avoid overriding data"});
      }
      if (not key_from_map_is_prefix(schemas, filename)) {
        gthrow(
            {"unknown file ", filename, "inside", directory.string(),
             "are you sure this is the right directory?"});
      }
    }
  }

  bool
  key_from_map_is_prefix(std::map<std::string, json> map, std::string string) {
    for (auto const &[name, _] : map) {
      if (string.rfind(name, 0) == 0) {
        return true;
      }
    }
    return false;
  }

  inline static json get_problem_data_schema() {
    // see docs/problem_data_schema.json
    return R"(
    {
      "$schema": "http://json-schema.org/draft-07/schema",
      "type": "object",
      "description": "Metadata of the Problem",
      "required": ["simulation_settings", "initial_values", "problem_data"],
      "properties": {
        "simulation_settings": {
          "type": "object",
          "description": "provides all data on time and newton solver",
          "required": [
            "use_simplified_newton",
            "maximal_number_of_newton_iterations",
            "tolerance",
            "retries",
            "start_time",
            "end_time",
            "desired_delta_t"
          ],
          "properties": {
            "use_simplified_newton": {"type": "boolean"},
            "maximal_number_of_newton_iterations": {"type": "integer", "minimum": 0},
            "tolerance": {"type": "number"},
            "retries": {"type": "integer", "minimum": 0},
            "start_time": {"type": "number"},
            "end_time": {"type": "number"},
            "desired_delta_t": {"type": "number"}
          }
        },
        "initial_values": {
          "type": "object",
          "description": "holds a file name to an additional initial values json sorted by subproblem",
          "required": ["subproblems"],
          "properties": {
            "subproblems": {
              "type": "object",
              "required": ["Network_problem"],
              "properties": {
                "Network_problem": {
                  "type": "object",
                  "required": ["initial_json"],
                  "properties": {
                    "initial_json": {
                      "type": "string",
                      "format": "iri-reference",
                      "description": "Initial Values JSON file"
                    }
                  }
                }
              }
            }
          }
        },
        "problem_data": {
          "type": "object",
          "description": "holds a file name to an additional initial values json sorted by subproblem",
          "required": ["subproblems"],
          "properties": {
            "subproblems": {
              "type": "object",
              "required": ["Network_problem"],
              "properties": {
                "Network_problem": {
                  "type": "object",
                  "required": ["defaults", "topology_json", "boundary_json", "control_json"],
                  "properties": {
                    "defaults": {
                      "type": "object",
                      "patternProperties": {"^.*$": {"type": "object"}},
                      "description": "Components With default Topology Properties"
                    },
                    "topology_json": {
                      "type": "string",
                      "format": "iri-reference",
                      "description": "Topology JSON file"
                    },
                    "boundary_json": {
                      "type": "string",
                      "format": "iri-reference",
                      "description": "Boundary JSON file"
                    },
                    "control": {
                      "type": "string",
                      "format": "iri-reference",
                      "description": "Control JSON file"
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    )"_json;
  }

} // namespace Aux::schema
