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
        ord_json["$schema"] = "../schemas/" + type + "_schema.json";
        aux_json::overwrite_json(file, ord_json);
      } else {
        std::cout << "Mising schema for: " << type << ", skipping!"
                  << std::endl;
      }
    }
    return 0;
  }
} // namespace Aux::schema