#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>

#include <make_schema.hpp>
#include <nlohmann/json.hpp>
#include <schema_validation.hpp>

using nlohmann::json;

class TestDefaults : public ::testing::Test {
public:
  const static json defaults_pipe;
  const static json schema_pipe;
  const static json pipe;
};

const json TestDefaults::defaults_pipe = R"(
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

const json TestDefaults::schema_pipe = R"(
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

const json TestDefaults::pipe = R"(
{
  "meta": "some string"
}
)"_json;

TEST_F(TestDefaults, happy_path) {
  json schema_pipe_copy = schema_pipe;
  json pipe_copy = pipe;
  Aux::schema::add_defaults(schema_pipe_copy, defaults_pipe);
  Aux::schema::apply_defaults(pipe_copy, schema_pipe_copy);
  EXPECT_NO_THROW(Aux::schema::validate_json(pipe_copy, schema_pipe_copy););
}