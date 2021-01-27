#include <Aux_json.hpp>
#include <Exception.hpp>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
namespace aux_json {


  void replace_entry_with_json_from_file(nlohmann::json & super_json, std::string const & key) {

    auto & sub_json = super_json["key"];
    if(sub_json.is_object()){
      return;
    } else if(sub_json.is_string()){
      auto new_sub_json = get_json_from_string(sub_json.get<std::string>());
    } else {
      gthrow({"The value at \"", key, "\" is neither a valid json object, nor a "
          "string pointing to a json file."});
    }
  }

  nlohmann::json get_json_from_string(std::string const &json_string) {
    nlohmann::json json_object;
    auto json_path = std::filesystem::path(json_string);
    if (!std::filesystem::exists((json_path))) {
      gthrow({"The file ", json_path.string(), " does not exist!"});
    }
    try {
      std::ifstream jsonfilestream(json_path);
      jsonfilestream >> json_object;
    } catch (...) {
      std::cout << __FILE__ << ":" << __LINE__
                << ": Couldn't load json from file: " << json_string
                << std::endl;
      throw;
    }
    return json_object;
  }

} // namespace aux_json
