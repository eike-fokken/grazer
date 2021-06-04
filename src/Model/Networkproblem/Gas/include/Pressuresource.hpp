#pragma once
#include "Pressureboundarynode.hpp"

namespace Model::Networkproblem::Gas {

  class Pressuresource final : public Pressureboundarynode {

  public:
    static std::string get_type();

    using Pressureboundarynode::Pressureboundarynode;
  };

} // namespace Model::Networkproblem::Gas
