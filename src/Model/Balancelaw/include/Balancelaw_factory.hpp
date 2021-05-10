#include "Pipe_Balancelaw.hpp"
#include <memory>
#include <string>

namespace Model::Balancelaw {

  std::unique_ptr<Pipe_Balancelaw>
  make_pipe_balancelaw(std::string const &balancelaw);
}
