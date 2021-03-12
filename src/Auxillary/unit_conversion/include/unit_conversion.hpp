#pragma once
#include <sstream>
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
    {"Hz", 1.0},
    {"per_min", 1.0/60.0},
    {"per_minute", 1.0/60.0}
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

  const std::map<std::string, double> volume_flux_units {
    {"m_cube_per_s", 1.0},
    {"1000m_cube_per_hour", 1000.0/3600.0}
  };

  // arbitrary lambda conversions
  typedef std::function<double (double)> conversion;

  // conversion instructions for temperature
  const std::map<std::string, conversion> temperature_units{
    {"K", [](double x) { return x; }},
    {"C", [](double x) { return x + 273.15; }},
    {"Celsius", [](double x) { return x + 273.15; }},
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


  /**
   * @brief In contrast to multiplicative conversions (e.g. feet to meter)
   * temperature conversion is more complicated. To allow simple multiplication
   * tables unit_conversion is overloaded to accept either a double or a lambda
   * function.
   * 
   * unit_conversion behaves like value*conv if conv is a double which covers
   * most unit conversions.
   * 
   * unit_conversion behaves like conv(value) if conv is a lambda which allows
   * for arbitarily complicated conversion.
   * 
   * @param value of the current unit
   * @param conv multiplicator to get to the si-unit
   * @return double si-unit
   */
  double unit_conversion(double value, double conv);

  /**
   * @brief In contrast to multiplicative conversions (e.g. feet to meter)
   * temperature conversion is more complicated. To allow simple multiplication
   * tables unit_conversion is overloaded to accept either a double or a lambda
   * function.
   * 
   * unit_conversion behaves like value*conv if conv is a double which covers
   * most unit conversions.
   * 
   * unit_conversion behaves like conv(value) if conv is a lambda which allows
   * for arbitarily complicated conversion.
   * 
   * @param value value which might have an arbitrary unit
   * @param conv lambda function which returns the si-unit
   * @return double si-unit
   */
  double unit_conversion(double value, conversion conv);

  /**
   * @brief parses a string which is the prefix of a unit (e.g. "500 k" of "500 km")
   * returns the value of the number multiplied with the value of the symbol which
   * is looked up in the prefix_map (e.g. an si prefix map (or a binary prefix map))
   * 
   * @param prefix the prefix string to parse
   * @param prefix_map the prefix_map which includes all the possible symbols
   * @return double the prefix value
   */
  double parse_prefix(
    std::string const &prefix, std::map<std::string, double> const &prefix_map
  );

  /**
   * @brief parses a string which is the prefix of a unit (e.g. "500 k" of "500 km")
   * returns the value of the number multiplied with the value of the si prefix
   * 
   * @param prefix the prefix string to parse
   * @return double the prefix value
   */
  double parse_prefix_si(std::string const &prefix);

  template<typename T> const std::tuple<std::string, T> parse_unit(
    std::string const &unit, 
    std::map<std::string, T> const &unit_map
  ) {
    auto unit_size = unit.size();
    for (auto const& [str_unit, val] : unit_map) {
      auto symb_size = str_unit.size();
      if (
        unit_size >= symb_size 
        and unit.compare(unit_size-symb_size, symb_size, str_unit) == 0
      ){ // match
        return std::tuple<std::string, T>(
          unit.substr(0, unit.size()-symb_size), 
          val
        );
      }
    }
    std::ostringstream o;
    o << "Could not find the unit of " << unit << " in the unit_map " << "\n";
    throw std::runtime_error(o.str());
  }


  template <typename T> double parse_to_si(json const &unit_json, std::map<std::string, T> const &unit_map){
    std::string unit = unit_json["unit"].get<std::string>();

    auto [prefix, conv] = parse_unit<T>(unit, unit_map);
    return unit_conversion(
      unit_json["value"].get<double>() * parse_prefix_si(prefix), conv
    );
  }

}
