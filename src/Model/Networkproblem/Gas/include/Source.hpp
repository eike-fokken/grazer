#pragma once
#include "Flowboundarynode.hpp"

namespace Model::Networkproblem::Gas {

  template<typename Flowboundarynode>
  class Source final : public Flowboundarynode {

  public:
    static std::string get_type();

    using Flowboundarynode::Flowboundarynode;
  };

  extern template class Source<Pressurecouplingnode>;
  extern template class Source<Bernoullicouplingnode>;

} // namespace Model::Networkproblem::Gas
