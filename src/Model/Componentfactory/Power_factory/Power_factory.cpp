#include "Power_factory.hpp"

#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  void add_power_components(Componentfactory &factory) {
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Power::Vphinode>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Power::PVnode>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Power::PQnode>>());

    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Power::Transmissionline>>());
  }

  Power_factory::Power_factory(){
    add_power_components(*this);
  }
}
