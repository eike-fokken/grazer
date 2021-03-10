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

  const std::map<std::string, double> area_units {
    {"m^2", 1.0},
    {"ac", 4046.9}, {"acre", 4046.9}
  };

  const std::map<std::string, double> volume_units {
    {"m^3", 1.0},
    {"L", 1e-3}, {"l", 1e-3}, {"\u2113", 1e-3},
  };

  const std::map<std::string, double> pressure_units {
    {"Pa", 1.0},
    {"bar", 1e5},
    {"atm", 101325},
    {"psi", 6894.757}
  };

  const std::map<std::string, double> frequency_units {
    {"Hz", 1.0}
  };

  const std::map<std::string, double> force_units {
    {"N", 1.0}
  };

  const std::map<std::string, double> power_units {
    {"W", 1.0},
    {"PS",  735.49875}
  };

  const std::map<std::string, double> mass_units {
    {"g", 0.001},
    {"t", 1000},
    {"oz", 0.028349523125},
    {"lb", 0.45359237}
  };

  struct Conversion {
    double shift; // shift in SI
    double slope;
  };

  const std::map<std::string, Conversion> temperature_units{
    {"K", {0.0, 1.0}},
    {"C", {273.15, 1.0}},
    {"\u2103", {273.15, 1.0}}, // degree celcius symbol
    {"\u00B0C", {273.15, 1.0}}, // degree symbol plus C
    {"F", {459.67 * 5.0 / 9.0, 5.0 / 9.0}},
    {"\u2109", {459.67 * 5.0 / 9.0, 5.0 / 9.0}},// degree fahrenheit symbol
    {"\u00B0F", {459.67 * 5.0 / 9.0, 5.0 / 9.0}}
  };

  /**
   * @brief for a unit string match the base unit from a unit_map return the 
   * prefix and the SI value of the base unit. 
   * E.g. parse_unit("km", Aux::unit::length_units ) -> ("k", 1.0)
   * parse_unit("10 ft", Aux::unit::length_units ) -> ("10 ", 0.3048)
   * 
   * ! Warning: do not use for units which are also shifted (e.g. temperature)
   * 
   * @param unit "[num][si_prefix][unit]"
   * @param unit_map e.g. {"m": 1.0, "in": 0.0254, ...}
   * @return const std::tuple<std::string, double>
   */
  const std::tuple<std::string, double> parse_unit(
    std::string const &unit, std::map<std::string, double> const &unit_map
  );

  /**
   * @brief for a unit string match the base unit from a unit_map return the 
   * prefix and the SI value of the base unit. 
   * E.g. parse_unit("C", Aux::unit::temperature_units) -> ("k", 1.0)
   * parse_unit("10 ft", Aux::unit::temperature_units ) -> ("10 ", 0.3048)
   * 
   * @param unit "[num][si_prefix][unit]"
   * @param unit_map e.g. {"m": 1.0, "in": 0.0254, ...}
   * @return const std::tuple<std::string, double>
   */
  const std::tuple<std::string, double> parse_unit(
    std::string const &unit, std::map<std::string, Conversion> const &unit_map
  );



  template <typename Conversiontype>
  double
  parse_to_si(json const &unit_json,
              std::map<std::string, Conversiontype> const &unit_map);

  extern template
  double parse_to_si(json const &unit_json, std::map<std::string, Conversion> const &unit_map);


  extern template double
  parse_to_si(json const &unit_json,
              std::map<std::string, double> const &unit_map);

  // /**
  //  * @brief parse a unit json into a unit with help of a unit_map
  //  * 
  //  * @param unit_json e.g. {"value": 10, "unit": "5 cm" }
  //  * @param unit_map e.g. {"m": 1.0, "yt": 0.0254}
  //  * @return double (e.g. with the values from above 0.5 [m = 10 * 5 cm])
  //  */
  // double parse_to_si(json const &unit_json, std::map<std::string, double> const &unit_map);
}
