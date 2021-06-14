#include <gmock/gmock-matchers.h>
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
  json ten_thousand_micro_metres_1000
      = json::parse("{\"value\": 1000,\"unit\": \"1e4 mum\"}");

  auto si_length = unit::length.parse_to_si(metres_10);

  EXPECT_EQ(si_length, 10.0);
  EXPECT_EQ(si_length, unit::length.parse_to_si(deca_metres_1));
  EXPECT_EQ(
      si_length, unit::length.parse_to_si(ten_thousand_micro_metres_1000));

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
      unit::temperature.parse_to_si(celcius),
      unit::temperature.parse_to_si(celcius_in_kelvin));
  EXPECT_EQ(
      unit::temperature.parse_to_si(celcius),
      unit::temperature.parse_to_si(celcius_in_fahrenheit));
}

TEST(unitParser, mostUsedUnits) {
  EXPECT_EQ(
      unit::pressure.parse_to_si(R"({"value": 0.1, "unit":"bar"})"_json),
      1e4 // Pa
  );
  EXPECT_EQ(
      unit::temperature.parse_to_si(R"({"value": 2, "unit":"Celsius"})"_json),
      275.15 // K
  );
  EXPECT_EQ(
      unit::temperature.parse_to_si(R"({"value": 234, "unit":"K"})"_json),
      234 // K
  );
  EXPECT_EQ(
      unit::length.parse_to_si(R"({"value": 23, "unit":"km"})"_json),
      23000 // m
  );
  EXPECT_EQ(
      unit::length.parse_to_si(R"({"value": 100, "unit": "mm"})"_json),
      0.1 // m
  );
  EXPECT_EQ(
      unit::power.parse_to_si(R"({"value": 23, "unit":"kW"})"_json),
      23000 // W
  );
  EXPECT_EQ(
      unit::volume_flux.parse_to_si(
          R"({"value": 1000, "unit": "m_cube_per_s"})"_json),

      unit::volume_flux.parse_to_si(
          R"({"value": 3600, "unit": "1000m_cube_per_hour"})"_json));
  EXPECT_EQ(
      unit::frequency.parse_to_si(R"({"value": 60, "unit": "per_min"})"_json),
      1.0 // Hz
  );
}

/**
 * @brief Tests Validation in json parsing and asserts that the json validation
 * error provides certain hints
 *
 * @param malformed_data json data to be parsed but malformed causing a json
 * validation error
 * @param required_hint string with the required error hints
 *
 */
void test_validation_in_parsing(json malformed_data, std::string required_hit);

/**
 * @brief finds required_hint in error_message and returns true if found
 *
 * @param error_message
 * @param required_hint
 * @return true
 * @return false
 */
bool error_msg_includes(std::string error_message, std::string required_hint);

TEST(unitParser, jsonValidation) {
  json wrong_name = R"({
    "not_value": 10,
    "unit": "m"
  })"_json;

  try {
    test_validation_in_parsing(
        wrong_name, "required property 'value' not found in object");
  } catch (std::exception &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr(
            "The current object json does not conform to its schema."));
  }

  json wrong_type = R"({
    "value": "string",
    "unit": "m"
  })"_json;

  try {
    test_validation_in_parsing(wrong_type, "unexpected instance type");
  } catch (std::exception &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr(
            "The current object json does not conform to its schema."));
  }

  json additional_property = R"({
    "value": "10",
    "unit": "m",
    "additionalProperty": "notAllowed"
  })"_json;

  try {
    test_validation_in_parsing(additional_property, "additional property");
  } catch (std::exception &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr(
            "The current object json does not conform to its schema."));
  }

  json unknown_unit = R"({
    "value": "10",
    "unit": "l"
  })"_json;

  try {
    test_validation_in_parsing(unknown_unit, "no subschema has succeeded");
  } catch (std::exception &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr(
            "The current object json does not conform to its schema."));
  }
}

void test_validation_in_parsing(
    json malformed_data, std::string required_hint) {
  try {
    unit::length.parse_to_si(malformed_data);
  } catch (const std::runtime_error &e) {
    std::string error_msg = e.what();
    EXPECT_PRED2(
        error_msg_includes, error_msg, "does not conform to its schema");
    EXPECT_PRED2(error_msg_includes, error_msg, required_hint);
    throw;
  }
}

bool error_msg_includes(std::string error_message, std::string required_hint) {
  return error_message.find(required_hint) != std::string::npos;
}
