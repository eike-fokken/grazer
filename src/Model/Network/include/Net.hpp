/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#pragma once
#include <memory>
#include <string>
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
    std::vector<Node const *> get_nodes() const;

    std::vector<Edge *> get_edges();
    std::vector<Edge const *> get_edges() const;
    /// This function indicates whether an edge between two nodes exists or
    /// not. The order of nodes in the edge is not relevant. Its reverse edge
    /// is thus also checked.
    /// @param id1 Starting Node of edge
    /// @param id2 Ending Node of edge
    /// @returns Return True if the given edge or its reverse edge exists and
    /// false if neither exist.
    bool
    exists_edge_between(std::string const &id1, std::string const &id2) const;

    /// This function returns (by providing the nodes id) a pointer of type
    /// <Node>.
    /// @param id Nodes id
    /// @returns Returns shared pointer of type <Node>
    Node *get_node_by_id(std::string const &id) const;

    Node *exists_node(Node *node) const;

    /// Similiar to Net::get_node_by_id(int) this function returns
    /// a shared pointer of type <Edge> given the corresponding nodes
    /// id. Mind the order!
    /// @param id1 Nodes id of starting node
    /// @param id2 Nodes id of ending node
    /// @returns Returns shared pointer of type < Edge >
    Edge *
    get_edge_by_node_ids(std::string const &id1, std::string const &id2) const;

    Edge *get_edge_by_id(std::string const &id) const;

  private:
    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Edge>> edges;
  };

} // namespace Network
