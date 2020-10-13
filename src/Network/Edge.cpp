#include <Edge.hpp>


#include <memory>

std::shared_ptr<Network::Node> Network::Edge::get_starting_node()
{
  return starting_node;
}

std::shared_ptr<Network::Node> Network::Edge::get_ending_node() {
  return ending_node;
}
