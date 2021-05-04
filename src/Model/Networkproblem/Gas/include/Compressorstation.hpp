#pragma once
#include "Shortpipe.hpp"

namespace Model::Networkproblem::Gas {

  class Compressorstation final : public Shortpipe {
  public:
    using Shortpipe::Shortpipe;
    static std::string get_type();
    std::string get_gas_type() const override;
    static nlohmann::json get_schema();
    void new_print_to_files(nlohmann::json &new_output) override;
  };
} // namespace Model::Networkproblem::Gas
