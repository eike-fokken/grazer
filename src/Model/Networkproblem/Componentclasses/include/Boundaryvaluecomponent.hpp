#pragma once
#include <nlohmann/json.hpp>

namespace Model {

  class Boundaryvaluecomponent {
  protected:
    Boundaryvaluecomponent(){};

  public:
    static std::optional<nlohmann::json> get_boundary_schema() = delete;
  };
} // namespace Model
