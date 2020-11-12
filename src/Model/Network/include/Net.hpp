#pragma once
#include "Edge.hpp"
#include "Node.hpp"
#include <memory>
#include <vector>

namespace Network {

  /// The class Net contains functions for creating nodes, connecting edges
  /// and also removing both.

  class Net {

  public:
    Net() = delete;
    Net(std::vector<std::shared_ptr<Node>> _nodes,
        std::vector<std::unique_ptr<Edge>> _edges);

    /// This function returns a vector of ids of all existing nodes in the
    /// net.
    /// @returns Vector of ids of the existing nodes
    std::vector<std::string> get_valid_node_names() const;

    std::vector<Node *> get_nodes();

    std::vector<Edge *> get_edges();
    /// This function indicates whether an edge between two nodes exists or
    /// not. The order of nodes in the edge is not relevant. Its reverse edge
    /// is thus also checked.
    /// @param node_id_1 Starting Node of edge
    /// @param node_id_2 Ending Node of edge
    /// @returns Return True if the given edge or its reverse edge exists and
    /// false if neither exist.
    bool exists_edge_between(std::string const name1,
                             std::string const name2) const;

    /// This function returns (by providing the nodes id) a pointer of type
    /// <Node>.
    /// @param id Nodes id
    /// @returns Returns shared pointer of type <Node>
    Node *get_node_by_name(std::string const name) const;

    Node *exists_node(Node *node) const;

    /// Similiar to Net::get_node_by_id(int) this function returns
    /// a shared pointer of type <Edge> given the corresponding nodes
    /// id. Mind the order!
    /// @param node_id_1 Nodes id of starting node
    /// @param node_id_2 Nodes id of ending node
    /// @returns Returns shared pointer of type < Edge >
    Edge *get_edge_by_node_names(std::string const name1,
                                 std::string const name2) const;

  private:
    std::vector<std::shared_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Edge>> edges;
  };

} // namespace Network
