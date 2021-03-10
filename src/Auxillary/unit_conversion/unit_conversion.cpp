#include <unit_conversion.hpp>

#include <sstream>
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

  double parse_unit(json const &pressure_json, std::string const &symbol) {
    double value = pressure_json["value"].get<double>();
    std::string unit = pressure_json["unit"].get<std::string>();

    // find symbol location in unit string
    auto symbol_location = unit.rfind(symbol);
    if (symbol_location == std::string::npos) { // not found?
      std::ostringstream o;
      o << "Missing unit symbol in " << unit << ", expected: " << symbol << "\n";
      throw std::runtime_error(o.str());
    }
    // split off prefix from symbol
    std::string prefix = unit.substr(0, symbol_location);

    // split into num_prefix and str_prefix
    std::string str_prefix;
    double num_prefix;

    std::string::size_type last_space = prefix.rfind(" ");
    if (last_space != std::string::npos) {
      std::string::size_type remaining;
      num_prefix = std::stod(prefix, &remaining);
      if (remaining != last_space) {
        throw std::runtime_error("Multiple spaces in unit not allowed. Format should be [(number) unit]");
      }
      str_prefix = prefix.substr(remaining);
    } else {
      num_prefix = 1.0;
      str_prefix = prefix;
    }

    // process str_prefix



    return value * num_prefix;
  }
}