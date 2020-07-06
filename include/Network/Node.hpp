#pragma once

#include<memory>
#inlcude<set>
#include<iostream>
#include <stdexcept>

namespace Network {


  class Edge;

  class Node
  {
  public:
    Node();
    Node(std::set<std::weak_ptr<Network::Edge>> start_edges,std::set<std::weak_ptr<Network::Edge>> end_edges):starting_edges(start_edges), ending_edges(end_edges) {};

    void remove_edge(std::weak_ptr<Network::Edge> to_remove);

    void attach_starting_edge(std::weak_ptr<Network::Edge> to_attach);

    void attach_ending_edge(std::weak_ptr<Network::Edge> to_attach);


  private:
    std::set<std::weak_ptr<Network::Edge>> starting_edges;
    std::set<std::weak_ptr<Network::Edge>> ending_edges;

  };

}
