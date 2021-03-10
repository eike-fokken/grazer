#pragma once
#include <nlohmann/json.hpp>

using nlohmann::json;

namespace Aux::unit {
  double parse_unit(json const &pressure_json, std::string const &symbol);
}