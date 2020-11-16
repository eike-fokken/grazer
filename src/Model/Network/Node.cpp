#include "Edge.hpp"
#include <Node.hpp>
#include <memory>
#include <vector>

bool Network::Node::remove_edge(Network::Edge *to_remove) {
  for (auto it = starting_edges.begin(); it != starting_edges.end(); ++it) {
    if ((*it) == to_remove) {
      starting_edges.erase(it);
      return true;
    }
  }

  for (auto it = ending_edges.begin(); it != ending_edges.end(); ++it) {
    if ((*it) == to_remove) {
      ending_edges.erase(it);
      return true;
    }
  }

  return false;
}

std::string Network::Node::get_id() const { return id; }

std::vector<Network::Edge *> Network::Node::get_starting_edges() const {
  return starting_edges;
}

std::vector<Network::Edge *> Network::Node::get_ending_edges() const {
  return ending_edges;
}

bool Network::Node::attach_starting_edge(Network::Edge *to_attach) {
  if (to_attach->get_starting_node() != this) {
    return false;
  }
  starting_edges.push_back(to_attach);
  return true;
}

bool Network::Node::attach_ending_edge(Network::Edge *to_attach) {
  if (to_attach->get_ending_node() != this) {
    return false;
  }
  ending_edges.push_back(to_attach);
  return true;
}
