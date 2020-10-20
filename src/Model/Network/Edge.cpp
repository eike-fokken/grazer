#include <Edge.hpp>

#include <memory>

std::shared_ptr<Network::Node> Network::Edge::get_starting_node() const {
  return starting_node;
}

std::shared_ptr<Network::Node> Network::Edge::get_ending_node() const {
  return ending_node;
}
