#pragma once
#include <nlohmann/json.hpp>
#include <filesystem>

namespace aux_json {
  /// \brief replaces a filename string with the json from the file
  ///
  /// Checks whether super_json["key"] holds a string. If so, checks that the
  /// string is a valid file name, reads the contents of the file of that name
  /// into a json object and replaces super_json["key"] with this json object.
  void replace_entry_with_json_from_file(nlohmann::json &super_json,
                                         std::string const & key);

  /// \brief Constructs a json object from a file.
  ///
  ///
  nlohmann::json get_json_from_file_path(std::filesystem::path const &file_path);



}
