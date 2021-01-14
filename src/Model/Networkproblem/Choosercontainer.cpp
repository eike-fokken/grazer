#include <Choosercontainer.hpp>

#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Vphinode.hpp"


namespace Model::Networkproblem::Netprob_Aux {

  Nodechooserset::Nodechooserset() {
    insert(std::make_unique<Specific_node_type_chooser<Power::Vphinode>>());
    insert(std::make_unique<Specific_node_type_chooser<Power::PVnode>>());
    insert(std::make_unique<Specific_node_type_chooser<Power::PQnode>>());
  }

} // namespace Model::Networkproblem::Netprob_Aux
