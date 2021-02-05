#pragma once
#include <string>
#include <memory>
#include <vector>

namespace Network {

  class Node;
  class Edge;

  /// The class Net contains functions for creating nodes, connecting edges
  /// and also removing both.

  class Net {

  public:
    Net() = delete;
    Net(std::vector<std::unique_ptr<Node>> _nodes,
        std::vector<std::unique_ptr<Edge>> _edges);

    /// This function returns a vector of ids of all existing nodes in the
    /// net.
    /// @returns Vector of ids of the existing nodes
    std::vector<std::string> get_valid_node_ids() const;

    std::vector<Node *> get_nodes();

    std::vector<Edge *> get_edges();
    /// This function indicates whether an edge between two nodes exists or
    /// not. The order of nodes in the edge is not relevant. Its reverse edge
    /// is thus also checked.
    /// @param node_id_1 Starting Node of edge
    /// @param node_id_2 Ending Node of edge
    /// @returns Return True if the given edge or its reverse edge exists and
    /// false if neither exist.
    bool exists_edge_between(std::string const id1,
                             std::string const id2) const;

    /// This function returns (by providing the nodes id) a pointer of type
    /// <Node>.
    /// @param id Nodes id
    /// @returns Returns shared pointer of type <Node>
    Node *get_node_by_id(std::string const id) const;

    Node *exists_node(Node *node) const;

    /// Similiar to Net::get_node_by_id(int) this function returns
    /// a shared pointer of type <Edge> given the corresponding nodes
    /// id. Mind the order!
    /// @param node_id_1 Nodes id of starting node
    /// @param node_id_2 Nodes id of ending node
    /// @returns Returns shared pointer of type < Edge >
    Edge *get_edge_by_node_ids(std::string const id1,
                               std::string const id2) const;

    Edge *get_edge_by_id(std::string const id) const;

    void display() const;

  private:
    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Edge>> edges;
  };

} // namespace Network
