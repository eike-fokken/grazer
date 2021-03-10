#pragma once
#include <nlohmann/json.hpp>

using nlohmann::json;

namespace Aux::unit {
  // convert double to boost::units::quantity<si::length> add "*si::meters" to the entries
  const std::map<std::string, double> lenght_units {
    {"m", 1.0},
    {"in", 0.0254},
    {"ft",  0.3048},
    {"yd", 0.9144},
    {"mi", 1609.344}
  };

  const std::tuple<std::string, double> parse_unit(
    std::string const &unit, std::map<std::string, double> const &unit_map
  );
  double parse(json const &unit_json, std::map<std::string, double> const &unit_map);
}