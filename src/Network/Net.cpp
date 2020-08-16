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

void Network::Net::remove_edge_between(int node_1,int node_2){
  for(int i = 0; i < edges.size(); i++){
    if ((edges[i] -> get_starting_node()-> get_id() == node_1) && (edges[i] -> get_ending_node() -> get_id() == node_2)){
        edges.erase(edges.begin()+i);
    }
  }
}

void Network::Net::remove_node(int node_id){
  for(int i = 0; i < nodes.size(); i++){
    if (nodes[i] -> get_id() == node_id){
        nodes.erase(nodes.begin()+i);
    }
  }
}

void Network::Net::make_edge_between(int start, int end) {
  if (exists_edge_between(start, end)) {
    std::cout << "this edge is already there!" << std::endl;
  } else {

    Network::Edge newedge(nodes[start], nodes[end]);
    auto edge_ptr = std::make_shared<Network::Edge>(newedge);
    nodes[start]->attach_starting_edge(edge_ptr);
    nodes[end]->attach_ending_edge(edge_ptr);
    edges.push_back(edge_ptr);
  }
}

std::shared_ptr<Network::Node> Network::Net::get_node_by_id(int id){
    return nodes[id];
}

std::vector<int> Network::Net::get_valid_node_ids() {
    
    std::vector<int> vector_of_nodes_ids;
    for(int i = 0; i < nodes.size(); ++i){
        int id = nodes[i] -> get_id();
        vector_of_nodes_ids.push_back(id);
    }
  return vector_of_nodes_ids;
}



std::shared_ptr<Network::Edge> Network::Net::get_edge_by_node_ids(int node_id_1, int node_id_2){

  for(int i = 0; i < edges.size(); i++){
    if ((edges[i] -> get_starting_node()-> get_id() == node_id_1) && (edges[i] -> get_ending_node() -> get_id() == node_id_2)){
      return edges[i];
    }
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

  auto it = std::find_if(edges.begin(), edges.end(), find_endpoints);
  if (it != edges.end()) {
    return true;
  } else {
    return false;
  }
}
