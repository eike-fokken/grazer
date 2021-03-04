#include "Powernode.hpp"
#include <Transmissionline.hpp>
#include <iostream>
#include <Exception.hpp>
#include <tuple>


namespace Model::Networkproblem::Power {

  std::string Transmissionline::get_type() { return "Transmissionline"; }

  bool Transmissionline::needs_boundary_values() { return false; }

  Transmissionline::Transmissionline(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> &nodes)
      : Edge(topology, nodes), G(std::stod(topology["G"].get<std::string>())),
        B(std::stod(topology["B"].get<std::string>())) {}

  double Transmissionline::get_G() const { return G; }

  double Transmissionline::get_B() const { return B; }

  Powernode * Transmissionline::get_starting_powernode() const {
    return dynamic_cast<Powernode *>(get_starting_node());
  }

  Powernode * Transmissionline::get_ending_powernode() const {
    return dynamic_cast<Powernode *>(get_ending_node());
  }

} // namespace Model::Networkproblem::Power
