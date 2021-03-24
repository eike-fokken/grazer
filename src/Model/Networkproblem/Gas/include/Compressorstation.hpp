#pragma once
#include "Shortpipe.hpp"

namespace Model::Networkproblem::Gas {

  class Compressorstation final : public Shortpipe {
  public:
    using Shortpipe::Shortpipe;
    static std::string get_type();
    static nlohmann::json get_schema();
  };
} // namespace Model::Networkproblem::Gas
