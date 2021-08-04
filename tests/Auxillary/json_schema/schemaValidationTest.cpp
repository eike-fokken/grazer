#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <nlohmann/json.hpp>
#include <schema_validation.hpp>

using nlohmann::json;

class JsonValidationData : public ::testing::Test {

public:
  const static json valid_schema;
  const static json valid_data;
  const static json missing_data;
  const static json invalid_data;
  const static json invalid_schema;
};

const json JsonValidationData::valid_schema = R"(
  {
    "$schema": "http://json-schema.org/draft-07/schema",
    "title": "Test Schema",
    "type": "object",
    "required": ["my_number"],
    "properties": {
      "my_number": {"type": "number", "default": 3},
      "my_string": {"type": "string"}
    }
  }
  )"_json;

const json JsonValidationData::valid_data = R"(
  {
    "$schema":  "my_test_schema",
    "title": "Valid Test Data",
    "my_number": 6.2
  }
)"_json;

const json JsonValidationData::missing_data = R"(
  {
    "$schema":  "my_test_schema",
    "title": "Missing Test Data",
    "my_string": "test"
  }
)"_json;

const json JsonValidationData::invalid_data = R"(
  {
    "$schema":  "my_test_schema",
    "title": "Invalid Test Data",
    "my_number": "test"
  }
)"_json;

const json JsonValidationData::invalid_schema
    = R"( {"type": "invalid Schema"} )"_json;

TEST_F(JsonValidationData, happyPath) {
  EXPECT_NO_THROW(Aux::schema::validate_json(valid_data, valid_schema));
}

TEST_F(JsonValidationData, wrongData) {
  try {
    Aux::schema::validate_json(missing_data, valid_schema);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("The json of the following object json "
                                     "does not conform to its schema."));
  }
  try {
    Aux::schema::validate_json(invalid_data, valid_schema);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("The json of the following object json "
                                     "does not conform to its schema."));
  }
  json missing_data_copy = missing_data;
  Aux::schema::apply_defaults(missing_data_copy, valid_schema);
  EXPECT_NO_THROW(Aux::schema::validate_json(missing_data_copy, valid_schema));
}

TEST_F(JsonValidationData, invalidSchema) {
  try {
    Aux::schema::validate_json(valid_data, invalid_schema);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("The json of the following object json "
                                     "does not conform to its schema."));
  }
}
