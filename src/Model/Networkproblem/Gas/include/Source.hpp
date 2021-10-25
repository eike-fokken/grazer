#pragma once
#include "Flowboundarynode.hpp"

namespace Model::Gas {

  class Source final : public Flowboundarynode {

  public:
    static std::string get_type();

    using Flowboundarynode::Flowboundarynode;
  };

} // namespace Model::Gas
