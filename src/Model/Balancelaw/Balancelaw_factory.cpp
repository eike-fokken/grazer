#include "Balancelaw_factory.hpp"
#include "Exception.hpp"
#include "Isothermaleulerequation.hpp"
#include <memory>

namespace Model::Balancelaw {

  std::unique_ptr<Pipe_Balancelaw>
  make_pipe_balancelaw(std::string const &balancelaw) {
    if (balancelaw == "Isothermaleulerequation") {
      return std::make_unique<Isothermaleulerequation>();
    } else {
      gthrow({"Unknown type of Balancelaw!"});
    }
  }
} // namespace Model::Balancelaw
