#pragma once
#include "Flowboundarynode.hpp"

namespace Model::Gas {

  nlohmann::json revert_boundary_conditions(nlohmann::json const &data);

  class Sink : public Flowboundarynode {

  public:
    static char const constexpr *get_type() { return "Sink"; }

    Sink(nlohmann::json const &data);
  };

} // namespace Model::Gas
