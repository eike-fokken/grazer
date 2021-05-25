#include <Aux_json.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <schema_key_insertion.hpp>

namespace Aux::schema {
  using nlohmann::ordered_json;
  int insert_schema_key_into_data_jsons(path const &grazer_directory) {
    auto schemas = grazer_directory / "schemas";
    auto problem_dir = grazer_directory / "problem";

    for (std::string const &type :
         {"boundary", "control", "initial", "topology"}) {
      if (std::filesystem::exists(schemas / (type + "_schema.json"))) {
        path file = problem_dir / (type + ".json");
        ordered_json ord_json = aux_json::get_ordered_json_from_file_path(file);
        ordered_json result;
        result["$schema"] = "../schemas/" + type + "_schema.json";
        for (auto &[key, value] : ord_json.items()) {
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