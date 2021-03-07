#pragma once
#include <nlohmann/json.hpp>

namespace nlohmann {
  bool validate_json(nlohmann::json const &data);
  bool validate_json(nlohmann::json const &data, std::string const &location);
}
