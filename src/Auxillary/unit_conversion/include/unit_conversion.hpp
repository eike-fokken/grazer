#pragma once
#include <nlohmann/json.hpp>
#include <sstream>

namespace Aux::unit {

  using nlohmann::json;

  class Conversion {
    const std::function<double(double)> conversion;

  public:
    Conversion(double multiplicator);
    Conversion(std::function<double(double)> conversion);
    double operator()(double value);
  };

  class Measure {
  public:
    const std::string name;

  private:
    const std::map<std::string, Conversion> conversion_map;

  public:
    Measure(
        const std::string name,
        const std::map<std::string, Conversion> conversion_map);
    json get_schema() const;
    double parse_to_si(json const unit) const;
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
