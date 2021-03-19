#include <make_schema.hpp>

namespace Aux::schema {
  nlohmann::json make_list_schema_of(nlohmann::json const &element_schema) {
    nlohmann::json list_schema;
    list_schema["type"] = "array";
    list_schema["items"] = element_schema;

    return list_schema;
  }
}
