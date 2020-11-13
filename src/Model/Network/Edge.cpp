#include <Edge.hpp>
#include <Exception.hpp>
#include <Node.hpp>
#include <memory>
#include <string>
namespace Network {

  Edge::Edge(std::string _name, Node *start_node, Node *end_node)
      : name(_name), starting_node(start_node), ending_node(end_node) {
    if (start_node == end_node) {
      gthrow("cant create an edge from a node to itself!");
    }
    start_node->attach_starting_edge(this);
    end_node->attach_ending_edge(this);
  }

  std::string Edge::get_name() { return name; }

  Node *Edge::get_starting_node() const { return starting_node; }

  Node *Edge::get_ending_node() const { return ending_node; }

} // namespace Network
