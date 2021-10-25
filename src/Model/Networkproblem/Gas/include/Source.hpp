#pragma once
#include "Flowboundarynode.hpp"

namespace Model::Gas {

  class Source final : public Flowboundarynode {

  public:
    static char const constexpr *get_type() { return "Source"; }

    using Flowboundarynode::Flowboundarynode;
  };

} // namespace Model::Gas
