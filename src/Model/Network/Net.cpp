#include "Edge.hpp"
#include <Exception.hpp>
#include <Net.hpp>

#include <algorithm>
#include <sstream>
#include <string>



  int Network::Net::new_node() {
    int id = highest_free_id;
    ++highest_free_id;
    std::shared_ptr<Network::Node> node_ptr (new Node(id));
    nodes.push_back(std::move(node_ptr));
    return id;
  }

  void Network::Net::remove_edge_between(int node_1, int node_2) {
    if (node_1 < 0 || node_2 < 0) {
      std::stringstream ss;
      ss << "Nodes can't have negative ids."  << node_1 << " and " << node_2 << std::endl;
      gthrow(ss.str().c_str());
    }


  for (auto itr = edges.begin(); itr != edges.end(); itr++) {
    if (((*itr)->get_starting_node()->get_id() == node_1) &&
        ((*itr)->get_ending_node()->get_id() == node_2)) {
      (*itr)->get_ending_node()->remove_edge((*itr));
      (*itr)->get_starting_node()->remove_edge((*itr));
      edges.erase(itr);

      return;
    }
  }
}

std::vector<std::shared_ptr<Network::Node>> Network::Net::get_nodes() {
  return nodes;
}

std::vector<std::shared_ptr<Network::Edge>> Network::Net::get_edges() {
  return edges;
}

void Network::Net::remove_node(int const id) {
  if (id < 0) {
    std::stringstream ss;
    ss << "Nodes can't have negative ids, node id is " << id << std::endl;
    gthrow(ss.str().c_str());
  }

  for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
    if ((*itr)->get_id() == id) {
      nodes.erase(itr);
    }
  }
}

void Network::Net::make_edge_between(int const start, int const end) {
  if (exists_edge_between(start, end)) {
    std::stringstream ss;
    ss << "There is already an edge between node " << start << " and " << end
       << std::endl;
    gthrow(ss.str().c_str());
  }

  auto start_ptr = get_node_by_id(start);
  if(!start_ptr)
    {gthrow("nullpointer start");}
  auto end_ptr = get_node_by_id(end);
  if (!end_ptr) {
    gthrow("nullpointer end");
  }

  std::shared_ptr<Network::Edge> edge_ptr(new Network::Edge(start_ptr,end_ptr));
  start_ptr->attach_starting_edge(edge_ptr);
  end_ptr->attach_ending_edge(edge_ptr);
  edges.push_back(std::move(edge_ptr));
}

std::shared_ptr<Network::Node> Network::Net::get_node_by_id(int const id) const {
  if (id < 0) {
    std::stringstream ss;
    ss << "Nodes can't have negative ids, node id is " << id << std::endl;
    gthrow(ss.str().c_str());
  }
  for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
    if ((*itr)->get_id() == id) {
      return (*itr);
    }
  }
    return std::shared_ptr<Network::Node>();
}
std::vector<int> Network::Net::get_valid_node_ids() const {

  std::vector<int> vector_of_nodes_ids;
  for (auto itr = nodes.begin(); itr != nodes.end(); itr++) {
    int id = (*itr)->get_id();
    vector_of_nodes_ids.push_back(id);
  }
  return vector_of_nodes_ids;
}

std::shared_ptr<Network::Edge>
Network::Net::get_edge_by_node_ids(int node_1, int node_2) const {

  if (node_1 < 0 || node_2 < 0) {
    gthrow("Nodes can't have negative ids.")
  }

  for (auto itr = edges.begin(); itr != edges.end(); itr++) {
    if (((*itr)->get_starting_node()->get_id() == node_1) &&
        ((*itr)->get_ending_node()->get_id() == node_2)) {
      return (*itr);
    }
  }

  return std::shared_ptr<Network::Edge>();
}

bool Network::Net::exists_edge_between(const int node_id_1,
                                       const int node_id_2) const {

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
