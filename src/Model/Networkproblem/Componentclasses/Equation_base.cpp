#include "Equation_base.hpp"

namespace Model {

  std::optional<nlohmann::json> Equation_base::get_boundary_schema() {
    return std::nullopt;
  }

} // namespace Model
