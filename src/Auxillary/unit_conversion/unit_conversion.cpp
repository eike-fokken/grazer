#include <unit_conversion.hpp>

#include <sstream>
#include <map>

namespace Aux::unit {
  // convert double to boost::units::quantity<si::length> add "*si::meters" to the entries
  const std::map<std::string, double> length_units {
    {"m", 1.0},
    {"in", 0.0254},
    {"ft",  0.3048},
    {"yd", 0.9144},
    {"mi", 1609.344}
  };
    json get_length_schema() {
    json type_schema;
    type_schema["type"] = "object";
    type_schema["description"] = "Length Measurement";
    type_schema["required"] = {"value","unit"};
    type_schema["additionalProperties"] = false;
    type_schema["properties"]["unit"]["type"] = "string";
    json pattern1;
    pattern1["pattern"]
        = "^(Y|Z|E|P|T|G|M|k|h|da|d|c|m|mu|\u00B5|\u03BC|n|p|f|a|z|y)?(m|ft|in|"
          "yd|mi)$";
    type_schema["properties"]["unit"]["oneOf"]
              .push_back(pattern1);
    json pattern2;
    pattern2["pattern"]
        = "^([0-9]*(\\.[0-9]+)? )(Y|Z|E|P|T|G|M|k|h|da|d|c|m|mu|\u00B5|\u03BC|n|"
          "p|f|a|z|y)?(m|ft|in|yd|mi)$";
    type_schema["properties"]["unit"]["oneOf"].push_back(pattern2);
    type_schema["properties"]["value"]["type"]= "number";
    return type_schema;
  }

  const std::map<std::string, double> area_units {
    {"m^2", 1.0},
    {"ac", 4046.9}, {"acre", 4046.9}
  };

  const std::map<std::string, double> volume_units {
    {"m^3", 1.0},
    {"L", 1e-3}, {"l", 1e-3}, {u8"\u2113", 1e-3},
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

  const std::map<std::string, conversion> temperature_units{
    {"K", [](double x) { return x; }},
    {"C", [](double x) { return x + 273.15; }},
    {"Celsius", [](double x) { return x + 273.15; }},
    {u8"\u2103", [](double x) { return x + 273.15; }},  // degree celcius symbol
    {u8"\u00B0C", [](double x) { return x + 273.15; }}, // degree symbol plus C
    {u8"F", [](double x) { return x * 5.0 / 9.0 + 459.67 * 5.0 / 9.0; }},
    {u8"\u2109", [](double x) { return x * 5.0 / 9.0 + 459.67 * 5.0 / 9.0; }}, // degree fahrenheit symbol
    {u8"\u00B0F", [](double x) { return x * 5.0 / 9.0 + 459.67 * 5.0 / 9.0; }}
  };


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
    {"mu", 1e-6}, {u8"\u00B5", 1e-6}, {u8"\u03BC", 1e-6}, //unicode micro and mu 
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
