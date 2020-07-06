#include<memory>

// To use the class one always has to pretend Network::

namespace Network {

  //This is a "forward declaration", so that one can use the Node class here:
  class Node;

  class Edge
  {
  public:
    //Creating an edge without also providing its start and end node is prohibited:
    Edge() = delete;
    // Instead creating the edge with start and end node is a good idea:
    Edge(std::shared_ptr<Network::Node> start_node,std::shared_ptr<Network::Node> end_node):starting_node(start_node),ending_node(end_node) {};



  private:
    //The edge holds a shared pointer to its start and end nodes.
    //This means a node will only be destroyed when the last edge connected to it is destroyed.
    std::shared_ptr<Network::Node> starting_node;
    std::shared_ptr<Network::Node> ending_node;

  };
}
