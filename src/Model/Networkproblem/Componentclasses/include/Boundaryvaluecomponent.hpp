#pragma once
#include <nlohmann/json.hpp>

namespace Model {

  class Boundaryvaluecomponent {
    static nlohmann::json get_boundary_schema();
  };
} // namespace Model
