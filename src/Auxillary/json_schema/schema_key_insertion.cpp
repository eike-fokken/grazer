#include <Aux_json.hpp>
#include <iostream>
#include <schema_key_insertion.hpp>

namespace Aux::schema {
  int insert_schema_key_into_data_jsons(path const &grazer_directory) {
    auto schemas = grazer_directory / "schemas";
    auto problem_dir = grazer_directory / "problem";

    for (std::string const &type :
         {"boundary", "control", "initial", "topology"}) {
      if (std::filesystem::exists(schemas / (type + "_schema.json"))) {
        path file = problem_dir / (type + ".json");
        auto problem_json = aux_json::get_json_from_file_path(file);
        problem_json["$schema"] = "../schemas/" + type + "_schema.json";
        aux_json::overwrite_json(file, problem_json);
      } else {
        std::cout << "Mising schema for: " << type << ", skipping!"
                  << std::endl;
      }
    }
    return 0;
  }
} // namespace Aux::schema