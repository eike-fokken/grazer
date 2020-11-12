#pragma once
#include <memory>

namespace Network {

  // This is a "forward declaration", so that one can use the Node class here:
  class Node;

  class Net;

  /// The class edge contains functions for getting information
  /// about the nodes attached to object Edge.
  class Edge {

  public:
    // The edge holds a shared pointer to its start and end nodes.
    // Creating an edge without also providing its start and
    // end node is prohibited:
    Edge(){};
    // Instead creating the edge with start and end node is a good idea:
    Edge(std::shared_ptr<Network::Node> start_node,
         std::shared_ptr<Network::Node> end_node)
        : starting_node(start_node), ending_node(end_node){};

    virtual ~Edge(){};

    /// Function returns starting node of Edge object.
    /// @returns Starting node of type std::shared_ptr<Network::Node>
    std::shared_ptr<Network::Node> get_starting_node() const;

    /// Function returns ending node of Edge object.
    /// @returns Ending node of type std::shared_ptr<Network::Node>
    std::shared_ptr<Network::Node> get_ending_node() const;

  private:
    std::shared_ptr<Network::Node> const starting_node;
    std::shared_ptr<Network::Node> const ending_node;
  };
} // namespace Network
