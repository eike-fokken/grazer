#pragma once
#include "Flowboundarynode.hpp"

namespace Model::Gas {

  nlohmann::json revert_boundary_conditions(nlohmann::json const &data);

  class Sink : public Flowboundarynode {

  public:
    static std::string get_type();

    Sink(nlohmann::json const &data);
  };

} // namespace Model::Gas
