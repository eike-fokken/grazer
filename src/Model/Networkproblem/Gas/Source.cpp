#include "Source.hpp"

namespace Model::Networkproblem::Gas {

  template <typename Flowboundarynode>
  std::string Source<Flowboundarynode>::get_type() {
    return "Source";
  }

 template class Source<Pressurecouplingnode>;
 template class Source<Bernoullicouplingnode>;
}
