#include "Gas_factory.hpp"

#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Innode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"

namespace Model::Componentfactory {

  void add_gas_components(
      Componentfactory &factory, nlohmann::json const &defaults) {
    factory.add_node_type(std::make_unique<NodeType<Gas::Source>>(defaults));
    factory.add_node_type(std::make_unique<NodeType<Gas::Sink>>(defaults));
    factory.add_node_type(std::make_unique<NodeType<Gas::Innode>>(defaults));

    factory.add_edge_type(std::make_unique<EdgeType<Gas::Pipe>>(defaults));
    factory.add_edge_type(std::make_unique<EdgeType<Gas::Shortpipe>>(defaults));
    factory.add_edge_type(
        std::make_unique<EdgeType<Gas::Controlvalve>>(defaults));
    factory.add_edge_type(
        std::make_unique<EdgeType<Gas::Compressorstation>>(defaults));
  }

  Gas_factory::Gas_factory(nlohmann::json const &defaults) {
    add_gas_components(*this, defaults);
  }
} // namespace Model::Componentfactory
