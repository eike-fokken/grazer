#include "make_schema.hpp"
#include "unit_conversion.hpp"
#include <stdexcept>

namespace Aux::schema {

  json type::simple(std::string type) {
    json type_schema;
    type_schema["type"] = type;
    return type_schema;
  }

  json type::simple(std::string type, std::string description) {
    json type_schema;
    type_schema["type"] = type;
    type_schema["description"] = description;
    return type_schema;
  }

  json
  type::simple(std::string type, std::string title, std::string description) {
    json type_schema;
    type_schema["type"] = type;
    type_schema["title"] = title;
    type_schema["description"] = description;
    return type_schema;
  }
  json type::number() { return type::simple("number"); }

  json type::number(std::string description) {
    return type::simple("number", description);
  }

  json type::number(std::string title, std::string description) {
    return type::simple("number", title, description);
  }

  json type::string() { return type::simple("string"); }

  json type::string(std::string description) {
    return type::simple("string", description);
  }

  json type::string(std::string title, std::string description) {
    return type::simple("string", title, description);
  }

  json type::length() { return Aux::unit::length.get_schema(); }

  nlohmann::json make_list_schema_of(nlohmann::json const &element_schema) {
    return make_list_schema_of(element_schema, 0, 0);
  }
  nlohmann::json make_list_schema_of(
      nlohmann::json const &element_schema, int min_items, int max_items) {
    nlohmann::json list_schema;
    list_schema["type"] = "array";
    list_schema["items"] = element_schema;

    if (min_items > 0) {
      list_schema["minItems"] = min_items;
    }
    if (max_items > 0) {
      list_schema["maxItems"] = max_items;
    }

    return list_schema;
  }

  nlohmann::json make_boundary_schema(int num_values) {
    nlohmann::json schema = R"({
      "type": "object",
      "required": ["id", "data"],
      "properties": {
        "id": {"type": "string"},
        "data": {}
      }
    })"_json;

    nlohmann::json data_element;
    data_element["time"] = type::number();
    data_element["values"]
        = make_list_schema_of(type::number(), num_values, num_values);

    schema["properties"]["data"] = make_list_schema_of(data_element);
    return schema;
  }

  void add_required(
      json &schema, std::string const &property_name,
      json const &property_def) {
    schema["required"].push_back(property_name);
    schema["properties"][property_name] = property_def;
  }

  void add_property(
      json &schema, std::string const &property_name,
      json const &property_def) {
    schema["properties"][property_name] = property_def;
  }

  json relax_schema(json schema, std::vector<std::string> keys) {
    for (auto const &key : keys) {
      auto it = std::find(
          schema["required"].begin(), schema["required"].end(), key);
      if (it != schema["required"].end()) {
        schema["required"].erase(it);
      }
    }
    return schema;
  }

} // namespace Aux::schema
