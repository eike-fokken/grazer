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


  template<typename T> const std::tuple<std::string, T> parse_unit(
    std::string const &unit, 
    std::map<std::string, T> const &unit_mult_map
  ) {
    auto unit_size = unit.size();
    for (auto const& [str_unit, val] : unit_mult_map) {
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

  double parse_prefix(
    std::string const &prefix, std::map<std::string, double> const &prefix_map
  );
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
  double parse_prefix_si(std::string const &prefix);
  double parse_prefix_si(std::string const &prefix){
    return parse_prefix(prefix, si_prefixes);
  }

  double parse_conv_si(json const &unit_json, std::map<std::string, Conversion> const &unit_map) {
    std::string unit = unit_json["unit"].get<std::string>();

    auto [prefix, conv] = parse_unit<Conversion>(unit, unit_map);
    return (unit_json["value"].get<double>() + parse_prefix_si(prefix))*conv.slope + conv.shift;
  }

  double parse_mult_si(json const &unit_json, std::map<std::string, double> const &unit_map) {
    std::string unit = unit_json["unit"].get<std::string>();

    auto [prefix, value] = parse_unit<double>(unit, unit_map);
    value *= unit_json["value"].get<double>();

    return value * parse_prefix_si(prefix);
  }
}