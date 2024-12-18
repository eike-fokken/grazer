#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <iostream>
#include <make_schema.hpp>
#include <nlohmann/json.hpp>
#include <schema_validation.hpp>

using nlohmann::json;

class TestDefaults : public ::testing::Test {
public:
  static json const defaults_pipe;
  static json const schema_pipe;
  static json const pipe_1;
  static json const pipe_2;
};

json const TestDefaults::defaults_pipe = R"(
{
  "meta": "test",
  "dimensions": {
    "length": {
      "unit": "cm",
      "value": 3
    }
  } 
} 
)"_json;

json const TestDefaults::schema_pipe = R"(
{
  "type": "object",
  "required": ["dimensions"],
  "properties": {
    "meta" : {"type": "string"},
    "dimensions": {
      "type": "object",
      "required": ["length"],
      "properties": {
        "length": {
          "type": "object",
          "required": ["unit", "value"],
          "properties": {
            "unit" : {"type": "string"},
            "value": {"type": "number"}
          }
        }
      }
    }
  }
}
)"_json;

json const TestDefaults::pipe_1 = R"(
{
  "meta": "some string"
}
)"_json;

json const TestDefaults::pipe_2 = R"(
{
  "dimensions": {
    "length": {
      "value": 2.1
    }
  }
}
)"_json;

TEST_F(TestDefaults, happy_path) {
  json schema_pipe_copy = schema_pipe;
  json pipe_1_copy = pipe_1;
  Aux::schema::add_defaults(schema_pipe_copy, defaults_pipe);
  Aux::schema::apply_defaults(pipe_1_copy, schema_pipe_copy);
  EXPECT_NO_THROW(Aux::schema::validate_json(pipe_1_copy, schema_pipe_copy););
  EXPECT_EQ(pipe_1_copy["meta"], "some string"); // keeps old values

  schema_pipe_copy = schema_pipe;
  json pipe_2_copy = pipe_2;
  Aux::schema::add_defaults(schema_pipe_copy, defaults_pipe);
  Aux::schema::apply_defaults(pipe_2_copy, schema_pipe_copy);
  EXPECT_NO_THROW(Aux::schema::validate_json(pipe_2_copy, schema_pipe_copy););
  EXPECT_EQ(pipe_2_copy["meta"], "test");
  EXPECT_EQ(pipe_2_copy["dimensions"]["length"]["value"], 2.1);
}

TEST_F(TestDefaults, too_many_defaults) {
  json defaults = R"({"unknown": "entry"})"_json;
  json schema_pipe_copy = schema_pipe;
  try {
    Aux::schema::add_defaults(schema_pipe_copy, defaults);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr("defaults contain the key \"unknown\", which is not "
                           "defined in the JSON Schema"));
  }
}