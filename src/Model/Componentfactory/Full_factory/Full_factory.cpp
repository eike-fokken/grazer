#include "Full_factory.hpp"
#include "ExternalPowerplant.hpp"
#include "Gas_factory.hpp"
#include "Power_factory.hpp"

#include "Compressorstation.hpp"
#include "Controlvalve.hpp"
#include "Gaspowerconnection.hpp"
#include "Innode.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "Sink.hpp"
#include "Source.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  void add_all_components(Componentfactory &factory) {
    add_power_components(factory);
    add_gas_components(factory);
    factory.add_edge_type(
        std::make_unique<EdgeType<
            Networkproblem::Gaspowerconnection::Gaspowerconnection>>());
    factory.add_node_type(
        std::make_unique<NodeType<
            Networkproblem::Gaspowerconnection::ExternalPowerplant>>());
  }

  Full_factory::Full_factory() { add_all_components(*this); }
} // namespace Model::Componentfactory
