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
        std::make_unique<NodeType<Networkproblem::Power::Vphinode>>(defaults));
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Power::PVnode>>(defaults));
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Power::PQnode>>(defaults));
    factory.add_node_type(
        std::make_unique<NodeType<Networkproblem::Power::StochasticPQnode>>(
            defaults));

    factory.add_edge_type(
        std::make_unique<EdgeType<Networkproblem::Power::Transmissionline>>(
            defaults));
  }

  Power_factory::Power_factory(nlohmann::json const &defaults) {
    add_power_components(*this, defaults);
  }
} // namespace Model::Componentfactory
