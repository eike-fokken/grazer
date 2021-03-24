#include "Powernode.hpp"
#include <Transmissionline.hpp>
#include <iostream>
#include <Exception.hpp>
#include <tuple>
#include "make_schema.hpp"

namespace Model::Networkproblem::Power {

  nlohmann::json Transmissionline::get_schema(){
    nlohmann::json schema = Network::Edge::get_schema();
    Aux::schema::add_required(schema, "B", Aux::schema::type::number());
    Aux::schema::add_required(schema, "G", Aux::schema::type::number());
    
    return schema;
  }

  std::string Transmissionline::get_type() { return "Transmissionline"; }

  bool Transmissionline::needs_boundary_values() { return false; }

  Transmissionline::Transmissionline(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes)
      : Edge(topology, nodes), G(topology["G"]),
        B(topology["B"]) {}

  double Transmissionline::get_G() const { return G; }

  double Transmissionline::get_B() const { return B; }

  Powernode * Transmissionline::get_starting_powernode() const {
    return dynamic_cast<Powernode *>(get_starting_node());
  }

  Powernode * Transmissionline::get_ending_powernode() const {
    return dynamic_cast<Powernode *>(get_ending_node());
  }

} // namespace Model::Networkproblem::Power
