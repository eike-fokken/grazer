#pragma once
#include "Shortpipe.hpp"

namespace Model::Gas {

  class Compressorstation final : public Shortpipe {
  public:
    using Shortpipe::Shortpipe;
    static std::string get_type();
    std::string get_gas_type() const override;
    static std::optional<nlohmann::json> get_control_schema();
    void add_results_to_json(nlohmann::json &new_output) override;
  };
} // namespace Model::Gas
