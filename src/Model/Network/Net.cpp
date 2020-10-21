#include <Net.hpp>
#include <Exception.hpp>

#include <algorithm>
#include <iostream>

unsigned int Network::Net::new_node() {
  unsigned int id = static_cast<unsigned int>(nodes.size());
  Network::Node node(id);
  auto node_ptr = std::make_shared<Network::Node>(node);
  nodes.push_back(node_ptr);
  return id;
}

void Network::Net::remove_edge_between(unsigned int node_1, unsigned int node_2) {
  for (unsigned int i = 0; i < edges.size(); i++) {
    if ((edges[i]->get_starting_node()->get_id() == node_1) &&
        (edges[i]->get_ending_node()->get_id() == node_2)) {
      edges.erase(edges.begin() + i);
    }
  }
}

void Network::Net::remove_node(unsigned int node_id) {
  for (unsigned int i = 0; i < nodes.size(); i++) {
    if (nodes[i]->get_id() == node_id) {
      nodes.erase(nodes.begin() + i);
    }
  }
}

void Network::Net::make_edge_between(unsigned int start, unsigned int end) {
  if (exists_edge_between(start, end)) {
    gthrow("There is already an edge!");
  } else {

    Network::Edge newedge(nodes[start], nodes[end]);
    auto edge_ptr = std::make_shared<Network::Edge>(newedge);
    nodes[start]->attach_starting_edge(edge_ptr);
    nodes[end]->attach_ending_edge(edge_ptr);
    edges.push_back(edge_ptr);
  }
}

std::shared_ptr<Network::Node> Network::Net::get_node_by_id(unsigned int id) const {
  return nodes[id];
}

std::vector<unsigned int> Network::Net::get_valid_node_ids() const {

  std::vector<unsigned int> vector_of_nodes_ids;
  for (unsigned int i = 0; i < nodes.size(); ++i) {
    unsigned int id = nodes[i]->get_id();
    vector_of_nodes_ids.push_back(id);
  }
  return vector_of_nodes_ids;
}

std::shared_ptr<Network::Edge>
Network::Net::get_edge_by_node_ids(unsigned int node_id_1, unsigned int node_id_2) const {

  for (unsigned int i = 0; i < edges.size(); i++) {
    if ((edges[i]->get_starting_node()->get_id() == node_id_1) &&
        (edges[i]->get_ending_node()->get_id() == node_id_2)) {
      return edges[i];
    }
  }
  std::shared_ptr<Network::Edge> nulledge;
  return nulledge;
}

bool Network::Net::exists_edge_between(const unsigned int node_id_1,
                                       const unsigned int node_id_2) const {

  auto find_endpoints = [node_id_1,
                         node_id_2](const std::shared_ptr<Network::Edge> edge) {
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

  auto it = std::find_if(edges.begin(), edges.end(), find_endpoints);
  if (it != edges.end()) {
    return true;
  } else {
    return false;
  }
}
