#include "Pipe_Balancelaw.hpp"
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Balancelaw {

  std::unique_ptr<Pipe_Balancelaw>
  make_pipe_balancelaw(nlohmann::json const & json);
}
