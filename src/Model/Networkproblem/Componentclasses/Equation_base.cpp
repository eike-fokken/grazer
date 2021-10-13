#include "Equation_base.hpp"

namespace Model {

  void Equation_base::setup() {}

  std::optional<nlohmann::json> Equation_base::get_boundary_schema() {
    return std::nullopt;
  }

} // namespace Model
