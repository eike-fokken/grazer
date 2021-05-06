#include <unit_conversion.hpp>

#include <map>
#include <sstream>

namespace Aux::unit {
  const Measure length{
      "length",
      {{"m", 1.0},
       {"in", 0.0254},
       {"ft", 0.3048},
       {"yd", 0.9144},
       {"mi", 1609.344}}};

  const Measure area{"area", {{"m^2", 1.0}, {"ac", 4046.9}, {"acre", 4046.9}}};

  const Measure volume{
      "volume",
      {
          {"m^3", 1.0},
          {"L", 1e-3},
          {"l", 1e-3},
      }};

  const Measure pressure{
      "pressure",
      {{"Pa", 1.0}, {"bar", 1e5}, {"atm", 101325}, {"psi", 6894.757}}};

  const Measure frequency{
      "frequency",
      {{"Hz", 1.0}, {"per_min", 1.0 / 60.0}, {"per_minute", 1.0 / 60.0}}};

  const Measure force{"force", {{"N", 1.0}}};

  const Measure power{"power", {{"W", 1.0}, {"PS", 735.49875}}};

  const Measure mass{
      "mass",
      {{"g", 0.001}, {"t", 1000}, {"oz", 0.028349523125}, {"lb", 0.45359237}}};

  const Measure volume_flux{
      "volume",
      {{"m_cube_per_s", 1.0}, {"1000m_cube_per_hour", 1000.0 / 3600.0}}};

  const Measure temperature{
      "temperature",
      {{"K", Conversion([](double x) { return x; })},
       {"C", Conversion([](double x) { return x + 273.15; })},
       {"Celsius", Conversion([](double x) { return x + 273.15; })},
       {"F", Conversion([](double x) {
          return x * 5.0 / 9.0 + 459.67 * 5.0 / 9.0;
        })}}};

  // https://en.wikipedia.org/wiki/Metric_prefix#List_of_SI_prefixes
  const std::map<std::string, Conversion> si_prefixes{
      {"Y", 1e24}, {"Z", 1e21},  {"E", 1e18},  {"P", 1e15},  {"T", 1e12},
      {"G", 1e9},  {"M", 1e6},   {"k", 1e3},   {"h", 100},   {"da", 10},
      {"", 1},     {"d", 0.1},   {"c", 1e-2},  {"m", 1e-3},  {"mu", 1e-6},
      {"n", 1e-9}, {"p", 1e-12}, {"f", 1e-15}, {"a", 1e-18}, {"z", 1e-21},
      {"y", 1e-24}};

  double parse_prefix(
      std::string const &prefix,
      std::map<std::string, Conversion> const &prefix_map) {
    // split into num_prefix and si_prefix
    std::string si_prefix;
    double num_prefix;

    std::string::size_type last_space = prefix.rfind(" ");
    if (last_space != std::string::npos) {
      std::string::size_type remaining;
      num_prefix = std::stod(prefix, &remaining);
      if (remaining != last_space) {
        throw std::runtime_error(
            "Multiple spaces in unit not allowed. Format should be [(number) "
            "unit]");
      }
      si_prefix = prefix.substr(remaining + 1);
    } else {
      num_prefix = 1.0;
      si_prefix = prefix;
    }

    auto search_si_prefix = prefix_map.find(si_prefix);
    if (search_si_prefix == prefix_map.end()) {
      std::ostringstream o;
      o << "Unknown si prefix: " << si_prefix << "\n";
      throw std::runtime_error(o.str());
    }
    Conversion conv = search_si_prefix->second;
    return conv(num_prefix);
  }

  double parse_prefix_si(std::string const &prefix) {
    return parse_prefix(prefix, si_prefixes);
  }

  Conversion::Conversion(double multiplicator) :
      conversion([multiplicator](double x) { return x * multiplicator; }) {}

  Conversion::Conversion(std::function<double(double)> conv) :
      conversion(conv) {}

  double Conversion::operator()(double value) {
    return this->conversion(value);
  }

  Measure::Measure(
      const std::string provided_name,
      const std::map<std::string, Conversion> conv_map) :
      name(provided_name), conversion_map(conv_map) {}

  double Measure::parse_to_si(json const unit_json) const {
    std::string unit = unit_json["unit"].get<std::string>();

    auto [prefix, conv] = parse_unit(unit, this->conversion_map);
    return conv(unit_json["value"].get<double>() * parse_prefix_si(prefix));
  }

  const std::tuple<std::string, Conversion> parse_unit(
      std::string const &unit,
      std::map<std::string, Conversion> const &unit_map) {
    auto unit_size = unit.size();
    for (auto const &[str_unit, val] : unit_map) {
      auto symb_size = str_unit.size();
      if (unit_size >= symb_size
          and unit.compare(unit_size - symb_size, symb_size, str_unit)
                  == 0) { // match
        return std::tuple<std::string, Conversion>(
            unit.substr(0, unit.size() - symb_size), val);
      }
    }
    std::ostringstream o;
    o << "Could not find the unit of " << unit << " in the unit_map "
      << "\n";
    throw std::runtime_error(o.str());
  }

  json Measure::get_schema() const {
    json type_schema;
    type_schema["type"] = "object";
    type_schema["description"] = this->name + "Measurement";
    type_schema["required"] = {"value", "unit"};
    type_schema["additionalProperties"] = false;
    type_schema["properties"]["unit"]["type"] = "string";
    type_schema["properties"]["value"]["type"] = "number";

    std::string simple_pattern = re_capture_from_map(si_prefixes) + "?"
                                 + re_capture_from_map(this->conversion_map)
                                 + "$";
    json pattern1;
    pattern1["pattern"] = "^" + simple_pattern;
    json pattern2;
    pattern2["pattern"] = "^([0-9]*(\\.[0-9]+)? )" + simple_pattern;
    type_schema["properties"]["unit"]["oneOf"].push_back(pattern1);
    type_schema["properties"]["unit"]["oneOf"].push_back(pattern2);
    return type_schema;
  }
  std::string re_capture_from_map(std::map<std::string, Conversion> map) {
    std::ostringstream regex_stream;
    regex_stream << "(";
    for (auto [item, _] : map) { regex_stream << item << "|"; }
    std::string regex = regex_stream.str();
    regex.pop_back();
    return regex + ")";
  }

} // namespace Aux::unit
