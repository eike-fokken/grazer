#include <make_schema.hpp>

namespace Aux::schema {

  nlohmann::json make_list_schema_of(nlohmann::json const &element_schema) {
    return make_list_schema_of(element_schema, 0, 0);
  }
  nlohmann::json make_list_schema_of(nlohmann::json const &element_schema, int min_items, int max_items) {
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

  nlohmann::json make_boundary_schema(int num_values){
    nlohmann::json schema = R"({
      "type": "object",
      "required": ["id", "data"],
      "properties": {
        "id": {"type": "string"},
        "data": {}
      }
    })"_json;

    nlohmann::json data_element;
    data_element["time"] = R"({"type": "numeric"})"_json;
    data_element["values"] = make_list_schema_of(
      R"({"type": "numeric"})"_json, num_values, num_values
    );

    schema["properties"]["data"] = make_list_schema_of(data_element);
    return schema;

  }
}
