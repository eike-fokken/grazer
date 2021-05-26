#include "Balancelaw_factory.hpp"
#include "Exception.hpp"
#include "Isothermaleulerequation.hpp"
#include <memory>

namespace Model::Balancelaw {

  std::unique_ptr<Pipe_Balancelaw>
  make_pipe_balancelaw(nlohmann::json const &json) {
    if (json["balancelaw"] == "Isothermaleulerequation") {
      return std::make_unique<Isothermaleulerequation>(json);
    } else {
      gthrow({"Unknown type of Balancelaw!"});
    }
  }
} // namespace Model::Balancelaw
