#include <unit_conversion.hpp>

#include <sstream>
#include <map>

namespace Aux::unit {
  // https://en.wikipedia.org/wiki/Metric_prefix#List_of_SI_prefixes
  const std::map<std::string, double> si_prefixes {
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
    {"", 1},
    {"d", 0.1}, 
    {"c", 1e-2}, 
    {"m", 1e-3},
    {"mu", 1e-6}, {"\u00B5", 1e-6}, {"\u03BC", 1e-6}, //unicode micro and mu 
    {"n", 1e-9},
    {"p", 1e-12},
    {"f", 1e-15},
    {"a", 1e-18},
    {"z", 1e-21},
    {"y", 1e-24}
  };

  double unit_conversion(double value, double unit) {
    return value * unit;
  }

  double unit_conversion(double value, conversion conv) {
    return conv(value);
  }

  double parse_prefix(
    std::string const &prefix, std::map<std::string,double> const &prefix_map
  ){
    // split into num_prefix and si_prefix
    std::string si_prefix;
    double num_prefix;

    std::string::size_type last_space = prefix.rfind(" ");
    if (last_space != std::string::npos) {
      std::string::size_type remaining;
      num_prefix = std::stod(prefix, &remaining);
      if (remaining != last_space) {
        throw std::runtime_error("Multiple spaces in unit not allowed. Format should be [(number) unit]");
      }
      si_prefix = prefix.substr(remaining+1);
    } else {
      num_prefix = 1.0;
      si_prefix = prefix;
    }

    auto search_si_prefix = prefix_map.find(si_prefix);
    if (search_si_prefix == prefix_map.end()) {
      std::ostringstream o;
      o << "Unknown si prefix: " << si_prefix <<"\n";
      throw std::runtime_error(o.str());
    } 
    return num_prefix * search_si_prefix->second;
  }

  double parse_prefix_si(std::string const &prefix){
    return parse_prefix(prefix, si_prefixes);
  }


  template double parse_to_si<conversion>(
    json const &unit_json,
    std::map<std::string, conversion> const &unit_map
  );
  template double parse_to_si<double>(
    json const &unit_json,
    std::map<std::string, double> const &unit_map
  );
}