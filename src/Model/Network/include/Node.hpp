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
#include "Idobject.hpp"
#include <vector>

namespace Network {

  class Net;

  class Edge;

  /// The class node contains all relevant functions for handling attachments
  /// and removement of edges as well as providing information about the nodes
  /// id.

  class Node : public Idobject {

  public:
    Node() = delete;

    Node(nlohmann::json const &data);

    virtual ~Node();

    /// This function attaches an edge to object of type node.
    /// Attention: Here object node has to be the starting node in the edge you
    /// provide.
    /// @param to_attach Edge to attach
    bool attach_starting_edge(Network::Edge *to_attach);

    /// This function attaches an edge to object of type node.
    /// Attention: Here object node has to be the ending node in the edge you
    /// provide.
    /// @param to_attach Edge to attach
    bool attach_ending_edge(Network::Edge *to_attach);

    /// This function removes the edge to_remove from the node.
    /// If successful it returns true.
    /// Else it does nothing but returns false.
    /// @param to_remove Edge that has to be removed
    bool remove_edge(Network::Edge *to_remove);

    /// This function returns vector pointers of starting edges
    std::vector<Network::Edge *> get_starting_edges() const;

    /// This function returns vector pointers of ending edges
    std::vector<Network::Edge *> get_ending_edges() const;

  private:
    struct Edgecollection {
      std::vector<Network::Edge *> starting_edges;
      std::vector<Network::Edge *> ending_edges;
    };
    std::unique_ptr<Edgecollection> attached_edges{
        std::make_unique<Edgecollection>()};
  };

} // namespace Network
