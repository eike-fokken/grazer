#pragma once
#include <nlohmann/json.hpp>

namespace Model {

  class Boundaryvaluecomponent {
  protected:
    Boundaryvaluecomponent(){};

  public:
    static nlohmann::json get_boundary_schema();
  };
} // namespace Model
