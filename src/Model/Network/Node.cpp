#include "Node.hpp"
#include "Edge.hpp"
#include "Exception.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace Network {
  std::string Node::get_type() {
    gthrow({"This static method must be implemented in the class inheriting "
            "from node!"});
  }

  Node::Node(nlohmann::json const &data)
      : Idobject(data["id"].get<std::string>()) {}

  bool Node::remove_edge(Edge *to_remove) {
    for (auto it =     attached_edges->starting_edges.begin(); it !=     attached_edges->starting_edges.end(); ++it) {
      if ((*it) == to_remove) {
            attached_edges->starting_edges.erase(it);
        return true;
      }
    }

    for (auto it =     attached_edges->ending_edges.begin(); it !=     attached_edges->ending_edges.end(); ++it) {
      if ((*it) == to_remove) {
            attached_edges->ending_edges.erase(it);
        return true;
      }
    }

    return false;
  }

  std::vector<Edge *> Node::get_starting_edges() const {
    return attached_edges->starting_edges;
  }

  std::vector<Edge *> Node::get_ending_edges() const {
    return attached_edges->ending_edges;
  }

  bool Node::attach_starting_edge(Edge *to_attach) {
    if (to_attach->get_starting_node() != this) {
      return false;
    }
        attached_edges->starting_edges.push_back(to_attach);
    return true;
  }

  bool Node::attach_ending_edge(Edge *to_attach) {
    if (to_attach->get_ending_node() != this) {
      return false;
    }
        attached_edges->ending_edges.push_back(to_attach);
    return true;
  }

} // namespace Network
