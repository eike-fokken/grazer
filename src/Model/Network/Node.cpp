#include "Node.hpp"
#include "Edge.hpp"
#include "Exception.hpp"
#include <iostream>
#include <memory>
#include <vector>

namespace Network {
  std::string Node::get_type() {
    gthrow({"This static method must be implemented in the class inherited "
            "from node!"});
  }

  Node::Node(nlohmann::json const &data)
      : Idobject(data["id"].get<std::string>()) {}

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

} // namespace Network
