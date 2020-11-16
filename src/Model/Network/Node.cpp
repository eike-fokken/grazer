#include "Edge.hpp"
#include <Node.hpp>
#include <iostream>
#include <memory>
#include <vector>

namespace Network {

  bool Node::remove_edge(Edge *to_remove) {
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

  std::string Node::get_id() const { return id; }

  std::vector<Edge *> Node::get_starting_edges() const {
    return starting_edges;
  }

  std::vector<Edge *> Node::get_ending_edges() const { return ending_edges; }

  bool Node::attach_starting_edge(Edge *to_attach) {
    if (to_attach->get_starting_node() != this) {
      return false;
    }
    starting_edges.push_back(to_attach);
    return true;
  }

  bool Node::attach_ending_edge(Edge *to_attach) {
    if (to_attach->get_ending_node() != this) {
      return false;
    }
    ending_edges.push_back(to_attach);
    return true;
  }

  void Node::display() { std::cout << "id: " << id << "\n"; }

  void Node::print_id() { std::cout << "id: " << id << ", "; }

} // namespace Network
