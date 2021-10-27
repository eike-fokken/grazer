#pragma once
#include "Flowboundarynode.hpp"

namespace Model::Gas {

  class Sink final : public Flowboundarynode {

  public:
    static std::string get_type();

    Sink(nlohmann::json const &data);
  };

} // namespace Model::Gas
