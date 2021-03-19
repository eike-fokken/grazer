#include <nlohmann/json.hpp>

namespace Aux::schema {
  nlohmann::json make_list_schema_of(nlohmann::json const &element_schema);
}