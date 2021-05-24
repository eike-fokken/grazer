#pragma once
#include <filesystem>
#include <nlohmann/json.hpp>

namespace Aux::schema {
  using nlohmann::json;
  using std::filesystem::path;

  int insert_schema_key_into_data_jsons(path const &grazer_directory);
} // namespace Aux::schema
