#include "../../include/Network/Net.hpp"
#include <algorithm>
#include <iostream>

int Network::Net::new_node() {
  int id = static_cast<int>(nodes.size());
  Network::Node node(id);
  auto node_ptr = std::make_shared<Network::Node>(node);
  nodes.push_back(node_ptr);
  return id;
}

void Network::Net::make_edge_between(int start, int end) {
  if (exists_edge_between(start, end)) {
    std::cout << "this edge is already there!" << std::endl;
  } else {
    Network::Edge newedge(nodes[start], nodes[end]);

    auto edge_ptr = std::make_shared<Network::Edge>(newedge);
    nodes[start]->attach_starting_edge(edge_ptr);
    nodes[end]->attach_ending_edge(edge_ptr);
  }
}

bool Network::Net::exists_edge_between(const int node_id_1, const int node_id_2) const {
  auto find_endpoints = [node_id_1, node_id_2](const std::shared_ptr<Network::Edge> edge) {
    if ((edge->get_starting_node()->get_id() == node_id_1) &&
        (edge->get_ending_node()->get_id() == node_id_2)) {
      return true;
    } else if ((edge->get_starting_node()->get_id() == node_id_2) &&
               (edge->get_ending_node()->get_id() == node_id_1)) {
      return true;
    } else {
      return false;
    }
  };

  std::find_if(edges.begin(), edges.end(), find_endpoints);
}
