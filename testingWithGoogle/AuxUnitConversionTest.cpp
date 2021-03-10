#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <unit_conversion.hpp>

namespace unit = Aux::unit;
using nlohmann::json;

TEST(unitParser, happyPath) {
  json metres_10 = R"({
    "value": 10,
    "unit": "m"
  })"_json; 
  json deca_metres_1 = R"({
    "value": 1,
    "unit": "dam"
  })"_json;
  json ten_thousand_micro_metres_1000 = json::parse(
    "{\"value\": 1000,\"unit\": \"1e4 \u00B5m\"}"
  );

  auto si_length = unit::parse_mult_si(metres_10, unit::lenght_units);

  EXPECT_EQ(si_length, 10.0);
  EXPECT_EQ(si_length, unit::parse_mult_si(deca_metres_1, unit::lenght_units));
  EXPECT_EQ(si_length, unit::parse_mult_si(ten_thousand_micro_metres_1000, unit::lenght_units));

  json celcius = R"({
    "value": 1,
    "unit": "C"
  })"_json; 

  json celcius_in_kelvin = R"({
    "value": 274.15,
    "unit": "K"
  })"_json;
  json celcius_in_fahrenheit = R"({
    "value": 33.8,
    "unit": "F"
  })"_json;

  EXPECT_EQ(
    unit::parse_conv_si(celcius, unit::temperature_units),
    unit::parse_conv_si(celcius_in_kelvin, unit::temperature_units)
  );
  EXPECT_EQ(
    unit::parse_conv_si(celcius, unit::temperature_units),
    unit::parse_conv_si(celcius_in_fahrenheit, unit::temperature_units)
  );
}