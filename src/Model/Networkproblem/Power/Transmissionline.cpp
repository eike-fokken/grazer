#include "Powernode.hpp"
#include <Transmissionline.hpp>
#include <iostream>
#include <Exception.hpp>
#include <tuple>


namespace Model::Networkproblem::Power {

  std::string Transmissionline::get_type() { return "Transmissionline"; }

  bool Transmissionline::needs_boundary_values() { return false; }

  Transmissionline::Transmissionline(std::string _id, Network::Node *start,
                                     Network::Node *end, double _G, double _B)
      : Edge(_id, start, end), G(_G), B(_B) {
    Network::Node *startnode = get_starting_node();
    auto start_powernode = dynamic_cast<Powernode *>(startnode);
    if(!start_powernode) {gthrow({"Transmissionline ", get_id(),  " has starting node ", startnode->get_id(), ",which is not a powernode!"});}
    Network::Node *endnode = get_ending_node();
    auto end_powernode = dynamic_cast<Powernode *>(endnode);
    if (!end_powernode) {
      gthrow({"Transmissionline ", get_id(), " has ending node ",
              endnode->get_id(), ",which is not a powernode!"});
    }
  }

  Transmissionline::Transmissionline(
      nlohmann::json const &topology,
      std::vector<std::unique_ptr<Network::Node>> & nodes)
      : Edge(topology, nodes), G(topology["G"]), B(topology["B"]) {}

  double Transmissionline::get_G() const { return G; }

  double Transmissionline::get_B() const { return B; }

  void Transmissionline::display() const {
    Edge::print_id();
    std::cout << "type: TL, G: " << G << ", B: " << B << "\n";
  }

  Powernode * Transmissionline::get_starting_powernode() const {
    return dynamic_cast<Powernode *>(get_starting_node());
  }

  Powernode * Transmissionline::get_ending_powernode() const {
    return dynamic_cast<Powernode *>(get_ending_node());
  }

} // namespace Model::Networkproblem::Power
