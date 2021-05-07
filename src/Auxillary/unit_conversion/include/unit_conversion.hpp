#pragma once
#include <nlohmann/json.hpp>
#include <sstream>

namespace Aux::unit {

  using nlohmann::json;

  /**
   * @brief Represents a Conversion of a value with one unit to the same value
   * with a different unit
   *
   * @details Examples: a conversion might be the conversion from kilometres to
   * metres this is achieved by taking x->1000*x. Applying this conversion conv
   * = Conversion(1000) to a number x thus results in conv(x) = 1000*x
   *
   * Conversions can constructed from doubles (which are interpreted as
   * multiplicators as above) but also from general functions. For the
   * conversion from fahrenheit to celcius for example one would need to supply
   * a general f_to_c function. conv = Conversion(f_to_c) acts like the supplied
   * function in this case: conv(x) = f_to_c(x).
   *
   * So the Conversion class allows the grouping of different conversion
   * methods only differening by the constructor they use.
   *
   */
  class Conversion {
    const double multiplicator;
    const double offset;

  public:
    Conversion(double multiplicator, double offset = 0);
    double operator()(double value);
  };

  /**
   * @brief Represents a measue (e.g. length). One can access its name, parse
   * json measurements to si units and get the json schema for such json
   * measurements.
   *
   */
  class Measure {
  public:
    const std::string name;

  private:
    const std::map<std::string, Conversion> conversion_map;

  public:
    Measure(
        const std::string name,
        const std::map<std::string, Conversion> conversion_map);
    /**
     * @brief Get the schema for a Json describing this Measurement
     *
     * @return json similar to {"value": {"type": "number"}, "unit": {"type":
     * "string", "pattern": <some_regex>}}
     */
    json get_schema() const;
    /**
     * @brief Converts a json measurement (described by the get_schema json
     * schema) to si units
     *
     * @param  json_measurement e.g. {"value": 123, "unit": "mm"}
     * @return double (value in si units) e.g. 0.123
     */
    double parse_to_si(json const json_measurement) const;
  };

  extern const Measure length;
  extern const Measure area;
  extern const Measure volume;
  extern const Measure pressure;
  extern const Measure frequency;
  extern const Measure force;
  extern const Measure power;
  extern const Measure mass;
  extern const Measure volume_flux;
  extern const Measure temperature;

  /**
   * @brief parses a string which is the prefix of a unit (e.g. "500 k" of "500
   * km") returns the value of the number multiplied with the value of the
   * symbol which is looked up in the prefix_map (e.g. an si prefix map (or a
   * binary prefix map))
   *
   * @param prefix the prefix string to parse
   * @param prefix_map the prefix_map which includes all the possible symbols
   * @return double the prefix value
   */
  double parse_prefix(
      std::string const &prefix,
      std::map<std::string, Conversion> const &prefix_map);

  /**
   * @brief parses a string which is the prefix of a unit (e.g. "500 k" of "500
   * km") returns the value of the number multiplied with the value of the si
   * prefix
   *
   * @param prefix the prefix string to parse
   * @return double the prefix value
   */
  double parse_prefix_si(std::string const &prefix);

  const std::tuple<std::string, Conversion> parse_unit(
      std::string const &unit,
      std::map<std::string, Conversion> const &unit_map);

  std::string re_capture_from_map(std::map<std::string, Conversion> map);
} // namespace Aux::unit
