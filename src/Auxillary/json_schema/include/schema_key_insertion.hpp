#pragma once
#include <filesystem>

namespace Aux::schema {
  using std::filesystem::path;

  int insert_schema_key_into_data_jsons(path const &grazer_directory);
} // namespace Aux::schema
