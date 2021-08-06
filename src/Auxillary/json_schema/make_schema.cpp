#include "make_schema.hpp"
#include "Exception.hpp"
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

  json type::boolean() { return type::simple("boolean"); }
  json type::boolean(std::string description) {
    return type::simple("boolean", description);
  }

  json type::boolean(std::string title, std::string description) {
    return type::simple("boolean", title, description);
  }

  nlohmann::json make_list_schema_of(nlohmann::json const &element_schema) {
    return make_list_schema_of(element_schema, R"({})"_json);
  }

  nlohmann::json make_list_schema_of(
      nlohmann::json const &element_schema,
      nlohmann::json const &array_params) {
    nlohmann::json list_schema;
    list_schema["type"] = "array";
    list_schema["items"] = element_schema;
    if (element_schema.contains("description")) {
      std::ostringstream stream;
      stream << "Array of " << element_schema["description"];
      list_schema["description"] = stream.str();
    }
    list_schema.update(array_params);
    return list_schema;
  }

  nlohmann::json make_initial_schema(
      std::optional<int> num_interpol_points, int num_values,
      std::vector<nlohmann::json> contains_interpol_point_schemas) {
    nlohmann::json initial_schema = R"(
      {
        "type": "object",
        "required": ["id", "data"],
        "properties": {
          "id": {
            "type": "string",
            "description": "component id"
          },
          "data": {}
        }
      }
      )"_json;

    nlohmann::json interpolation_point = R"(
      {
        "type": "object",
        "description": "Interpolation Point with Initial Values"
      }
      )"_json;

    add_required(interpolation_point, "x", type::number("Interpolation Point"));
    add_required(
        interpolation_point, "values",
        make_list_schema_of(
            type::number(),
            {{"description", "Initial Values at Interpolation Point x"},
             {"minItems", num_values},
             {"maxItems", num_values}}));

    nlohmann::json interpol_point_array_params;
    interpol_point_array_params["description"]
        = "Array of Interpolation Points with Initial Values";
    if (num_interpol_points.has_value()) {
      if (num_interpol_points <= 0) {
        gthrow(
            {"You can't demand a non-positive number of initial values.\n",
             "The definition of get_initial_schema is wrong.\n",
             "If you want to not force the number of initial values, return "
             "std::nullopt."})
      }
      interpol_point_array_params["minItems"] = num_interpol_points.value();
      interpol_point_array_params["maxItems"] = num_interpol_points.value();
    }
    nlohmann::json data
        = make_list_schema_of(interpolation_point, interpol_point_array_params);
    data["allOf"] = R"([])"_json;

    for (auto const &x_schema : contains_interpol_point_schemas) {
      nlohmann::json all_of_elmt;
      all_of_elmt["contains"]["properties"]["x"] = x_schema;
      data["allOf"].push_back(all_of_elmt);
    }

    initial_schema["properties"]["data"] = data;

    return initial_schema;
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
    data_element["values"] = make_list_schema_of(
        type::number(), {{"minItems", num_values}, {"maxItems", num_values}});

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

  static void add_defaults_unsafe(json &properties, json const &defaults) {
    // assumes that a "properties" json is provided, use the safe add_defaults!
    for (auto &[key, _] : defaults.items()) {
      if (not properties.contains(key)) {
        std::ostringstream o;
        o << "The defaults contain the key \"" << key
          << "\", which is not defined in the JSON Schema!\n\n"
          << "Schema Dump:\n"
          << properties;
        throw std::runtime_error(o.str());
      }
    }

    for (auto &[name, definition] : properties.items()) {
      if (not(definition.type() == json::value_t::object)) {
        std::ostringstream o;
        o << "Property Definition is not a JSON Object!\n";
        o << "Dump of Properties:\n\n" << properties;
        std::runtime_error(o.str());
      }
      if (not definition.contains("type")) {
        std::ostringstream o;
        o << "\"type\" attribute is missing from property!\n";
        o << "Dump of Properties:\n\n" << properties;
        std::runtime_error(o.str());
      }
      if (defaults.contains(name)) {
        definition["default"] = defaults[name];
        if ((definition["type"] == "object")
            and (definition.contains("properties"))) {
          if (not(defaults[name].type() == json::value_t::object)) {
            std::ostringstream o;
            o << "Property " << name
              << " is an object according to the JSON schema, but the default "
                 "definition is not a JSON object.\n"
              << "Dump Defaults:\n\n"
              << defaults << "\n\n"
              << "Dump Schema:\n\n"
              << properties;
          }
          // recursion
          add_defaults_unsafe(definition["properties"], defaults[name]);
        }
      }
    }
  }

  void add_defaults(json &schema, json const &defaults) {
    if ((schema.type() == json::value_t::object) and schema.contains("type")
        and (schema["type"] == "object") and (schema.contains("properties"))
        and (defaults.type() == json::value_t::object)) {
      add_defaults_unsafe(schema["properties"], defaults);
      return;
    }
    // figure out what went wrong and print error message
    std::ostringstream o;
    if (not(schema.type() == json::value_t::object)) {
      o << "Schema is not a JSON object!\n\n";
    } else if (not schema.contains("type")) {
      o << "Schema does not contain the key type!\n\n";
    } else if (schema["type"] != "object") {
      o << "Schema type is not \"object\"\n\n";
    } else if (not schema.contains("properties")) {
      o << "Schema does not contain \"properties\" key\n\n";
    } else if (not(defaults.type() == json::value_t::object)) {
      o << "Defaults are not a JSON object!\n"
        << "Dump of Defaults:\n\n"
        << defaults << "\n\n";
    }
    o << "Dump of Schema:\n\n" << schema;
    throw std::runtime_error(o.str());
  }

  void remove_defaults_from_required(json &schema) {
    if ((schema.type() == json::value_t::object) and schema.contains("type")
        and (schema["type"] == "object") and schema.contains("properties")) {
      if (schema.contains("required")) {
        auto required_list = schema["required"];
        if (not(required_list.type() == json::value_t::array)) {
          gthrow({"Required List is not a List"}) // TODO: better errmsg + test
                                                  // elements are strings
        }
        required_list.erase( // resizes the vector using the first empty element
                             // returned by remove_if and the current vector end
            std::remove_if(
                // removes all matching elements and fills the gaps by shifting
                // the remaining elements forward (does not resize!)
                required_list.begin(), required_list.end(),
                [&schema](std::string req_property) { // condition lambda
                  return schema["properties"]
                      .value(req_property, R"({})"_json)
                      .contains("default");
                }),
            required_list.end());
      }
      for (auto &[_, definition] : schema["properties"].items()) {
        // recursion
        remove_defaults_from_required(definition);
      }
    }
    return;
  }

} // namespace Aux::schema
