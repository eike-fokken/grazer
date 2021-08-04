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

  static void add_defaults_unsafe(json &properties, json const &defaults) {
    // assumes that a "properties" json is provided, use the safe add_defaults
    // instead
    for (auto &[name, definition] : properties.items()) {
      if (defaults.contains(name)) {
        definition["default"] = defaults[name];
      }
      if ((definition["type"] == "object")
          and (definition.contains("properties"))) {
        // recursion
        add_defaults_unsafe(definition["properties"], defaults);
      }
    }
  }

  void add_defaults(json &schema, json const &defaults) {
    if ((schema.type() == json::value_t::object) and schema.contains("type")
        and (schema["type"] == "object") and (schema.contains("properties"))) {
      add_defaults_unsafe(schema["properties"], defaults);
    }
  }

} // namespace Aux::schema
