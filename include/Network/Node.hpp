#pragma once
#include<memory>
#inlcude<set>


namespace Network {


  class Edge;

  class Node
  {
  public:
    // Default constructor not allowed:
    Node() = delete;

    //Constructor must at least provide an id:k
    Node(int _id):id(_id) {};
    //Destructor:
    ~Node();

    void attach_starting_edge(std::shared_ptr<Network::Edge> to_attach);

    void attach_ending_edge(std::shared_ptr<Network::Edge> to_attach);

    int get_id();
    bool has_id(const int id);
    



    
  private:
    int id;
    std::set<std::weak_ptr<Network::Edge>> starting_edges;
    std::set<std::weak_ptr<Network::Edge>> ending_edges;

  };

}
