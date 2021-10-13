#include "Power_factory.hpp"

#include "PQnode.hpp"

#include "PVnode.hpp"
#include "StochasticPQnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

namespace Model::Componentfactory {

  void add_power_components(
      Componentfactory &factory, nlohmann::json const &defaults) {
    factory.add_node_type(
        std::make_unique<NodeType<Power::Vphinode>>(defaults));
    factory.add_node_type(
        std::make_unique<NodeType<Power::PVnode>>(defaults));
    factory.add_node_type(
        std::make_unique<NodeType<Power::PQnode>>(defaults));
    factory.add_node_type(
        std::make_unique<NodeType<Power::StochasticPQnode>>(
            defaults));

    factory.add_edge_type(
        std::make_unique<EdgeType<Power::Transmissionline>>(
            defaults));
  }

  Power_factory::Power_factory(nlohmann::json const &defaults) {
    add_power_components(*this, defaults);
  }
} // namespace Model::Componentfactory
