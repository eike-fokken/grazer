#include "Threepointscheme.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Scheme {

  std::unique_ptr<Scheme::Threepointscheme>
  make_threepointscheme(nlohmann::json const &json);
}
