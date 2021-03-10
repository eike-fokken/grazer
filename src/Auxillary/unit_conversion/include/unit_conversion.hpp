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

  /**
   * @brief for a unit string match the base unit from a unit_map return the 
   * prefix and the SI value of the base unit. 
   * E.g. parse_unit("km", Aux::unit::length_units ) -> ("k", 1.0)
   * parse_unit("10 ft", Aux::unit::length_units ) -> ("10 ", 0.3048)
   * 
   * @param unit "[num][si_prefix][unit]"
   * @param unit_map e.g. {"m": 1.0, "in": 0.0254, ...}
   * @return const std::tuple<std::string, double>
   */
  const std::tuple<std::string, double> parse_unit(
    std::string const &unit, std::map<std::string, double> const &unit_map
  );


  /**
   * @brief parse a unit json into a unit with help of a unit_map
   * 
   * @param unit_json e.g. {"value": 10, "unit": "5 cm" }
   * @param unit_map e.g. {"m": 1.0, "yt": 0.0254}
   * @return double (e.g. with the values from above 0.5 [m = 10 * 5 cm])
   */
  double parse(json const &unit_json, std::map<std::string, double> const &unit_map);
}