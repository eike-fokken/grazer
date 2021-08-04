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

  void
  add_all_components(Componentfactory &factory, nlohmann::json const defaults) {
    add_power_components(factory, defaults);
    add_gas_components(factory, defaults);
    factory.add_edge_type(
        std::make_unique<
            EdgeType<Networkproblem::Gaspowerconnection::Gaspowerconnection>>(
            defaults));
    factory.add_node_type(
        std::make_unique<
            NodeType<Networkproblem::Gaspowerconnection::ExternalPowerplant>>(
            defaults));
  }

  Full_factory::Full_factory(nlohmann::json const defaults) {
    add_all_components(*this, defaults);
  }
} // namespace Model::Componentfactory
