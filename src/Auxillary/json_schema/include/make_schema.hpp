#pragma once
#include <nlohmann/json.hpp>

namespace Aux::schema {
  nlohmann::json make_list_schema_of(nlohmann::json const &element_schema);
  nlohmann::json make_list_schema_of(nlohmann::json const &element_schema, int min_items, int max_items);

  /**
   * @brief creates a schema for boundary values
   * 
   * @param num_values number of values per timestamp
   * @return nlohmann::json 
   */
  nlohmann::json make_boundary_schema(int num_values);
}