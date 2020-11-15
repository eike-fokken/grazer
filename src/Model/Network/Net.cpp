#include "Edge.hpp"
#include <Exception.hpp>
#include <Net.hpp>
#include <Node.hpp>

#include <algorithm>
#include <sstream>
#include <string>

namespace Network {

  Net::Net(std::vector<std::unique_ptr<Network::Node>> _nodes,
           std::vector<std::unique_ptr<Network::Edge>> _edges)
      : nodes(std::move(_nodes)), edges(std::move(_edges)) {
    for (auto &edge : edges) {
      {
        auto start = edge->get_starting_node();
        if (!start) {
          gthrow({"edge doesn't have a starting node!"});
        }

        auto node_in_net = exists_node(start);
        if (!node_in_net) {
          auto name = start->get_name();
          gthrow({"The node ", name,
                  "is not part of the net object but attached to an edge!",
                  "\n"});
        }
      }
      {
        auto end = edge->get_ending_node();
        if (!end) {
          gthrow({"edge does not have an ending node!"});
        }

        auto node_in_net = exists_node(end);
        if (!node_in_net) {
          auto name = end->get_name();
          gthrow({"The node ", name,
                  "is not part of the net object but attached to an edge!",
                  "\n"});
        }
      }
    }
  }

  std::vector<Node *> Net::get_nodes() {
    std::vector<Node *> raw_nodes;
    for (auto &node_sptr : nodes) {
      raw_nodes.push_back(node_sptr.get());
    }
    return raw_nodes;
  }

  std::vector<Edge *> Net::get_edges() {
    std::vector<Edge *> raw_edges;
    for (auto &edge_uptr : edges) {
      raw_edges.push_back(edge_uptr.get());
    }
    return raw_edges;
  }

  Node *Net::get_node_by_name(std::string const name) const {
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
      if ((*itr)->get_name() == name) {
        return (*itr).get();
      }
    }
    return nullptr;
  }

  Node *Net::exists_node(Node *node) const {
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
      if ((*itr).get() == node) {
        return (*itr).get();
      }
    }
    return nullptr;
  }

  std::vector<std::string> Net::get_valid_node_names() const {
    std::vector<std::string> vector_of_node_names;
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
      auto name = (*itr)->get_name();
      vector_of_node_names.push_back(name);
    }
    return vector_of_node_names;
  }

  Edge *Net::get_edge_by_node_names(std::string const name1,
                                    std::string const name2) const {
    for (auto itr = edges.begin(); itr != edges.end(); itr++) {
      if (((*itr)->get_starting_node()->get_name() == name1) &&
          ((*itr)->get_ending_node()->get_name() == name2)) {
        return (*itr).get();
      }
    }

    return nullptr;
  }

  Edge *Net::get_edge_by_name(std::string const name) const {

    for (auto &edge : edges) {
      if (edge->get_name() == name) {
        return edge.get();
      }
    }

    return nullptr;
  }

  bool Net::exists_edge_between(std::string const name1,
                                std::string const name2) const {

    auto find_endpoints = [name1, name2](std::unique_ptr<Edge> const &edge) {
      if ((edge->get_starting_node()->get_name() == name1) &&
          (edge->get_ending_node()->get_name() == name2)) {
        return true;
      } else if ((edge->get_starting_node()->get_name() == name2) &&
                 (edge->get_ending_node()->get_name() == name1)) {
        return true;
      } else {
        return false;
      }
    };
    auto it = std::find_if(edges.begin(), edges.end(), find_endpoints);
    if (it != edges.end()) {
      return true;
    } else {
      return false;
    }
  }
} // namespace Network
