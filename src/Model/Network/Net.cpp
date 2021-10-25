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
    for (auto &edge : edges) {
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
          for (auto &edgeptr : node_in_net->get_starting_edges()) {
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
          for (auto &edgeptr : node_in_net->get_ending_edges()) {
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
    for (auto &node_sptr : nodes) { raw_nodes.push_back(node_sptr.get()); }
    return raw_nodes;
  }

  std::vector<Node const *> Net::get_nodes() const {
    std::vector<Node const *> raw_nodes;
    for (auto &node_sptr : nodes) { raw_nodes.push_back(node_sptr.get()); }
    return raw_nodes;
  }

  std::vector<Edge *> Net::get_edges() {
    std::vector<Edge *> raw_edges;
    for (auto &edge_uptr : edges) { raw_edges.push_back(edge_uptr.get()); }
    return raw_edges;
  }

  std::vector<Edge const *> Net::get_edges() const {
    std::vector<Edge const *> raw_edges;
    for (auto &edge_uptr : edges) { raw_edges.push_back(edge_uptr.get()); }
    return raw_edges;
  }

  Node *Net::get_node_by_id(std::string const &id) const {
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
      if ((*itr)->get_id() == id) {
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

  std::vector<std::string> Net::get_valid_node_ids() const {
    std::vector<std::string> vector_of_node_ids;
    for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
      auto id = (*itr)->get_id();
      vector_of_node_ids.push_back(id);
    }
    return vector_of_node_ids;
  }

  Edge *Net::get_edge_by_node_ids(
      std::string const &id1, std::string const &id2) const {
    for (auto itr = edges.begin(); itr != edges.end(); itr++) {
      if (((*itr)->get_starting_node()->get_id() == id1)
          && ((*itr)->get_ending_node()->get_id() == id2)) {
        return (*itr).get();
      }
    }

    return nullptr;
  }

  Edge *Net::get_edge_by_id(std::string const &id) const {

    for (auto &edge : edges) {
      if (edge->get_id() == id) {
        return edge.get();
      }
    }

    return nullptr;
  }

  bool Net::exists_edge_between(
      std::string const &id1, std::string const &id2) const {

    auto find_endpoints = [id1, id2](std::unique_ptr<Edge> const &edge) {
      if ((edge->get_starting_node()->get_id() == id1)
          && (edge->get_ending_node()->get_id() == id2)) {
        return true;
      } else if (
          (edge->get_starting_node()->get_id() == id2)
          && (edge->get_ending_node()->get_id() == id1)) {
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
