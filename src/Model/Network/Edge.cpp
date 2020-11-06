#include <Edge.hpp>
#include <Node.hpp>
#include <memory>

std::shared_ptr<Network::Node> Network::Edge::get_starting_node() const {
  return starting_node;
}

void Network::Edge::set_starting_node(std::shared_ptr<Network::Node> node) {
  auto old_starting_node = get_starting_node();
  if (old_starting_node) {
    old_starting_node->remove_edge(std::make_shared<Network::Edge>(*this));
  }
  node->attach_starting_edge(std::make_shared<Network::Edge>(*this));
}

std::shared_ptr<Network::Node> Network::Edge::get_ending_node() const {
  return ending_node;
}

void Network::Edge::set_ending_node(std::shared_ptr<Network::Node> node) {
  auto old_ending_node = get_ending_node();
  if (old_ending_node) {
    old_ending_node->remove_edge(std::make_shared<Network::Edge>(*this));
  }
  node->attach_ending_edge(std::make_shared<Network::Edge>(*this));
}
