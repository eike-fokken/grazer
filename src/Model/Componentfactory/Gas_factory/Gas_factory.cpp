#include "Gas_factory.hpp"

#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Innode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"

namespace Model::Componentfactory {

  void add_gas_components(Componentfactory &factory) {
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Gas::Source>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Gas::Sink>>());
    factory.add_node_type(std::make_unique<NodeType<Networkproblem::Gas::Innode>>());

    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Pipe>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Shortpipe>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Controlvalve>>());
    factory.add_edge_type(std::make_unique<EdgeType<Networkproblem::Gas::Compressorstation>>());
  }

  Gas_factory::Gas_factory(){
    add_gas_components(*this);
  }
}
