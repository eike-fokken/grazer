#include <unit_conversion.hpp>
#include <map>

namespace Aux::unit {
  // https://en.wikipedia.org/wiki/Metric_prefix#List_of_SI_prefixes
  // convert double to boost::units::quantity<si::length> add "*si::meters" to the entries
  std::map<std::string, double> si_prefixes {
    {"Y", 1e24},
    {"Z", 1e21},
    {"E", 1e18},
    {"P", 1e15},
    {"T", 1e12},
    {"G", 1e9},
    {"M", 1e6},
    {"k", 1e3}, 
    {"h", 100}, 
    {"da", 10}, 
    {"d", 0.1}, 
    {"c", 1e-2}, 
    {"m", 1e-3},
    {"mu", 1e-3}, {"\xB5", 1e-3}, //unicode micro sign
    {"n", 1e-3},
    {"p", 1e-3},
    {"f", 1e-3},
    {"a", 1e-3},
    {"z", 1e-3},
    {"y", 1e-3}
  };

  double si_pressure(json const &pressure_json) {
    double value = pressure_json["value"].get<double>();
    std::string unit = pressure_json["unit"].get<std::string>();

    // split into num_unit and string_unit
    std::string string_unit;
    double num_unit;

    std::string::size_type last_space = unit.rfind(" ");
    if (last_space != std::string::npos) {
      std::string::size_type remaining;
      num_unit = std::stod(unit, &remaining);
      if (remaining != last_space) {
        throw std::runtime_error("Multiple spaces in unit not allowed. Format should be [(number) unit]");
      }
      string_unit = unit.substr(remaining);
    } else {
      num_unit = 1.0;
      string_unit = unit;
    }

    // process string_unit

    return value;
  }
}