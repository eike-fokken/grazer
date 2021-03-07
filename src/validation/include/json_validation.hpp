#pragma once
#include <nlohmann/json.hpp>

namespace nlohmann {
  bool validate_json(nlohmann::json data);
  bool validate_json(nlohmann::json data, std::string location);
}
