#include <Aux_json.hpp>
#include <Exception.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
namespace aux_json {

  void replace_entry_with_json_from_file(
      nlohmann::json &super_json, std::string const &key) {

    if (!super_json.is_object()) {
      gthrow(
          {"The json ", super_json.dump(),
           " is not a json object! There is probably a mistake in a json ",
           "file."});
    }

    auto &sub_json = super_json[key];
    if (sub_json.is_object()) {
      return;
    } else if (sub_json.is_string()) {
      auto json_pathstring = sub_json.get<std::string>();
      auto json_path = std::filesystem::path(json_pathstring);
      if (json_path.is_absolute()) {
        sub_json = get_json_from_file_path(json_path.string());
        return;
      } else { // json_path is relative
        if (!super_json.contains("GRAZER_file_directory")) {
          gthrow(
              {"The json ", super_json.dump(),
               " doesn't contain the key 'GRAZER_file_directory.\n",
               "This is a bug. Please file a bug report.\n"});
        }
        auto directory_path = std::filesystem::path(
            super_json["GRAZER_file_directory"].get<std::string>());
        auto json_full_path = directory_path / json_path;
        sub_json = get_json_from_file_path(json_full_path.string());
        return;
      }

    } else {
      gthrow(
          {"The value, ", sub_json.dump(4), " at \"", key,
           "\" is neither a valid json object, nor a ",
           "string pointing to a json file.\n It is given by ",
           sub_json.dump(4)});
    }
  }

  nlohmann::json
  get_json_from_file_path(std::filesystem::path const &file_path) {
    nlohmann::json json_object;
    if (not std::filesystem::exists(file_path)) {
      gthrow(
          {"The file \n", std::filesystem::absolute(file_path).string(),
           "\n does not exist!"});
    }
    try {
      std::ifstream jsonfilestream(file_path);
      jsonfilestream >> json_object;
    } catch (...) {
      std::cout << __FILE__ << ":" << __LINE__
                << ": Couldn't load json from file: " << file_path << std::endl;
      throw;
    }
    return json_object;
  }

  void overwrite_json(std::filesystem::path const &file_path, nlohmann::json const &target) {
    
  }

} // namespace aux_json
