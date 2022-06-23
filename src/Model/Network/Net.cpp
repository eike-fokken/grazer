#include <Edge.hpp>
#include <Exception.hpp>
#include <Net.hpp>
#include <Node.hpp>

#include <algorithm>
#include <string>

namespace Network {

  Net::Net(
      std::vector<std::unique_ptr<Network::Node>> _nodes,
      std::vector<std::unique_ptr<Network::Edge>> _edges) :
      nodes(std::move(_nodes)), edges(std::move(_edges)) {
    for (auto const &edge : edges) {
      {
        auto start = edge->get_starting_node();
        if (!start) {
          gthrow({"edge doesn't have a starting node!"});
        }

        auto node_in_net = exists_node(start);
        if (!node_in_net) {
          auto id = start->get_id();
          gthrow(
              {"The node ", id,
               "is not part of the net object but attached to an edge!", "\n"});
        } else {
          bool found(false);
          for (auto const &edgeptr : node_in_net->get_starting_edges()) {
            if (edgeptr == edge.get()) {
              found = true;
            }
          }
          if (!found) {
            gthrow({"edge not attached at its starting node!"});
          }
        }
      }
      {
        auto end = edge->get_ending_node();
        if (!end) {
          gthrow({"edge does not have an ending node!"});
        }

        auto node_in_net = exists_node(end);
        if (!node_in_net) {
          auto id = end->get_id();
          gthrow(
              {"The node ", id,
               "is not part of the net object but attached to an edge!", "\n"});
        } else {
          bool found(false);
          for (auto const &edgeptr : node_in_net->get_ending_edges()) {
            if (edgeptr == edge.get()) {
              found = true;
            }
          }
          if (!found) {
            gthrow({"edge not attached at its ending node!"});
          }
        }
      }
    }
  }

  std::vector<Node *> Net::get_nodes() {
    std::vector<Node *> raw_nodes;
    for (auto const &node_unique_ptr : nodes) {
      raw_nodes.push_back(node_unique_ptr.get());
    }
    return raw_nodes;
  }

  std::vector<Node const *> Net::get_nodes() const {
    std::vector<Node const *> raw_nodes;
    for (auto const &node_unique_ptr : nodes) {
      raw_nodes.push_back(node_unique_ptr.get());
    }
    return raw_nodes;
  }

  std::vector<Edge *> Net::get_edges() {
    std::vector<Edge *> raw_edges;
    for (auto const &edge_unique_ptr : edges) {
      raw_edges.push_back(edge_unique_ptr.get());
    }
    return raw_edges;
  }

  std::vector<Edge const *> Net::get_edges() const {
    std::vector<Edge const *> raw_edges;
    for (auto const &edge_unique_ptr : edges) {
      raw_edges.push_back(edge_unique_ptr.get());
    }
    return raw_edges;
  }

  Node *Net::get_node_by_id(std::string const &id) const {
    auto compare_id = [&id](std::unique_ptr<Node> const &node) -> bool {
      return id == node->get_id();
    };
    auto node_iterator = std::find_if(nodes.begin(), nodes.end(), compare_id);
    if (node_iterator == nodes.end()) {
      return nullptr;
    }
    return node_iterator->get();
  }

  Node *Net::exists_node(Node *node) const {
    auto compare_rawpointer
        = [node](std::unique_ptr<Node> const &node_ptr) -> bool {
      return node == node_ptr.get();
    };
    auto node_iterator
        = std::find_if(nodes.begin(), nodes.end(), compare_rawpointer);
    if (node_iterator == nodes.end()) {
      return nullptr;
    }
    return node_iterator->get();
  }

  std::vector<std::string> Net::get_valid_node_ids() const {
    std::vector<std::string> vector_of_node_ids;
    for (auto const &node_ptr : nodes) {
      auto id = node_ptr->get_id();
      vector_of_node_ids.push_back(id);
    }
    return vector_of_node_ids;
  }

  Edge *Net::get_edge_by_node_ids(
      std::string const &id1, std::string const &id2) const {

    auto compare_endpoints
        = [&id1, &id2](std::unique_ptr<Edge> const &edge_ptr) -> bool {
      return (edge_ptr->get_starting_node()->get_id() == id1)
             and (edge_ptr->get_ending_node()->get_id() == id2);
    };
    auto edge_iterator
        = std::find_if(edges.begin(), edges.end(), compare_endpoints);

    if (edge_iterator == edges.end()) {
      return nullptr;
    }
    return edge_iterator->get();
  }

  Edge *Net::get_edge_by_id(std::string const &id) const {

    auto compare_id = [&id](std::unique_ptr<Edge> const &edge) -> bool {
      return id == edge->get_id();
    };
    auto edge_iterator = std::find_if(edges.begin(), edges.end(), compare_id);
    if (edge_iterator == edges.end()) {
      return nullptr;
    }
    return edge_iterator->get();
  }

  bool Net::exists_edge_between(
      std::string const &id1, std::string const &id2) const {

    auto find_endpoints = [id1, id2](std::unique_ptr<Edge> const &edge) {
      // starting at id1 and ending at id2:
      return ((edge->get_starting_node()->get_id() == id1)
              and (edge->get_ending_node()->get_id() == id2))
             // starting at id2 and ending at id1:
             or ((edge->get_starting_node()->get_id() == id2)
                 and (edge->get_ending_node()->get_id() == id1));
    };

    auto edge_iterator
        = std::find_if(edges.begin(), edges.end(), find_endpoints);

    return edge_iterator != edges.end();
  }

} // namespace Network
