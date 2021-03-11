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

  typedef std::function<double (double)> conversion;
  const std::map<std::string, conversion> temperature_units{
    {"K", [](double x) { return x; }},
    {"C", [](double x) { return x + 273.15; }},
    {"\u2103", [](double x) { return x + 273.15; }},  // degree celcius symbol
    {"\u00B0C", [](double x) { return x + 273.15; }}, // degree symbol plus C
    {"F", [](double x) { return x * 5.0 / 9.0 + 459.67 * 5.0 / 9.0; }},
    {"\u2109", [](double x) { return x * 5.0 / 9.0 + 459.67 * 5.0 / 9.0; }}, // degree fahrenheit symbol
    {"\u00B0F", [](double x) { return x * 5.0 / 9.0 + 459.67 * 5.0 / 9.0; }}
  };

  template <typename Conversiontype>
  double parse_to_si(
    json const &unit_json,
    std::map<std::string, Conversiontype> const &unit_map
  );

  extern template
  double parse_to_si<conversion>(
    json const &unit_json,
    std::map<std::string, conversion> const &unit_map
  );


  extern template 
  double parse_to_si<double>(
    json const &unit_json,
    std::map<std::string, double> const &unit_map
  );

}
