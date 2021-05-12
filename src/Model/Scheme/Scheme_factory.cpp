#include "Scheme_factory.hpp"
#include "Exception.hpp"
#include "Implicitboxscheme.hpp"
#include <memory>

namespace Model::Scheme {

  std::unique_ptr<Threepointscheme>
  make_threepointscheme(nlohmann::json const &json) {
    if (json["scheme"] == "Implicitboxscheme") {
      return std::make_unique<Implicitboxscheme>();
    } else {
      gthrow({"Unknown type of scheme!"});
    }
  }
} // namespace Model::Scheme
