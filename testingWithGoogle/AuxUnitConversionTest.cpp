#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <unit_conversion.hpp>

namespace unit = Aux::unit;
using nlohmann::json;

TEST(unitParser, parsingWorks) {
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

  auto si_length = unit::parse_to_si(metres_10, unit::lenght_units);

  EXPECT_EQ(si_length, 10.0);
  EXPECT_EQ(si_length, unit::parse_to_si(deca_metres_1, unit::lenght_units));
  EXPECT_EQ(si_length, unit::parse_to_si(ten_thousand_micro_metres_1000, unit::lenght_units));

  json celcius = R"({
    "value": 1,
    "unit": "C"
  })"_json; 

  json celcius_in_kelvin = R"({
    "value": 274.15,
    "unit": "K"
  })"_json;
  json celcius_in_fahrenheit = R"({
    "value": 3.38,
    "unit": "10 F"
  })"_json;

  EXPECT_EQ(
    unit::parse_to_si(celcius, unit::temperature_units),
    unit::parse_to_si(celcius_in_kelvin, unit::temperature_units)
  );
  EXPECT_EQ(
    unit::parse_to_si(celcius, unit::temperature_units),
    unit::parse_to_si(celcius_in_fahrenheit, unit::temperature_units)
  );
}

TEST(unitParser, mostUsedUnits) {
  EXPECT_EQ(
    unit::parse_to_si(R"({"value": 0.1, "unit":"bar"})"_json, unit::pressure_units), 
    1e4 // Pa
  );
  EXPECT_EQ(
    unit::parse_to_si(R"({"value": 2, "unit":"Celsius"})"_json, unit::temperature_units),
    275.15 // K
  );
  EXPECT_EQ(
    unit::parse_to_si(R"({"value": 234, "unit":"K"})"_json, unit::temperature_units),
    234 // K
  );
  EXPECT_EQ(
    unit::parse_to_si(R"({"value": 23, "unit":"km"})"_json, unit::lenght_units),
    23000 // m
  );
  EXPECT_EQ(
    unit::parse_to_si(R"({"value": 100, "unit": "mm"})"_json, unit::lenght_units),
    0.1 // m
  );
  EXPECT_EQ(
    unit::parse_to_si(R"({"value": 23, "unit":"kW"})"_json, unit::power_units),
    23000 // W
  );
  EXPECT_EQ(
    unit::parse_to_si(R"({"value": 1000, "unit": "m_cube_per_s"})"_json, unit::volume_flux_units),
    unit::parse_to_si(R"({"value": 3600, "unit": "1000m_cube_per_hour"})"_json, unit::volume_flux_units)
  );
  EXPECT_EQ(
    unit::parse_to_si(R"({"value": 60, "unit": "per_min"})"_json, unit::frequency_units),
    1.0 // Hz
  );
  

}