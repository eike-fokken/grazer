#pragma once
#include "Flowboundarynode.hpp"

namespace Model::Networkproblem::Gas {

  class Source final : public Flowboundarynode {

  public:
    static std::string get_type();

    using Flowboundarynode::Flowboundarynode;
  };

} // namespace Model::Networkproblem::Gas
