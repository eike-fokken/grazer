#pragma once
#include<memory>
#include<vector>

namespace Network {

  class Net;

  class Edge;

  class Node
  {
  public:
    // Default constructor not allowed:
    Node() = delete;

    //Constructor must at least provide an id:k
    Node(int _id):id(_id) {};


    void attach_starting_edge(std::shared_ptr<Network::Edge> to_attach);

    void attach_ending_edge(std::shared_ptr<Network::Edge> to_attach);

    void remove_edge(std::shared_ptr<Network::Edge> to_remove);

    std::vector<std::weak_ptr<Network::Edge> > get_starting_edges();
    std::vector<std::weak_ptr<Network::Edge> > get_ending_edges();

    int get_id();
    bool has_id(const int id);
    
  private:
    int id;
    std::vector<std::weak_ptr<Network::Edge> > starting_edges;
    std::vector<std::weak_ptr<Network::Edge> > ending_edges;

    //friend class Network::Net;
  };

}
