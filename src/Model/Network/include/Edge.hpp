/*
 * Grazer - network simulation tool
 *
 * Copyright 2020-2021 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	AGPL-3.0-or-later
 *
 * Licensed under the GNU Affero General Public License v3.0, found in
 * LICENSE.txt and at https://www.gnu.org/licenses/agpl-3.0.html
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */

#pragma once
#include <Idobject.hpp>

namespace Network {

  // This is a "forward declaration", so that one can use the Node class here:
  class Node;

  class Net;

  /// The class edge contains functions for getting information
  /// about the nodes attached to object Edge.
  class Edge : public Idobject {

  public:
    static std::string get_type();
    static nlohmann::json get_schema();

    // The edge holds a shared pointer to its start and end nodes.
    // Creating an edge without also providing its start and
    // end node is prohibited:
    Edge() = delete;

    /// \brief Builds an edge from an appropriate json AND registers it in the
    /// start and end nodes.

    Edge(
        nlohmann::json const &edge_json,
        std::vector<std::unique_ptr<Node>> &nodes);

    virtual ~Edge(){};

    /// Function returns starting node of Edge object.
    /// @returns Starting node of type std::shared_ptr<Network::Node>
    Node *get_starting_node() const;

    /// Function returns ending node of Edge object.
    /// @returns Ending node of type std::shared_ptr<Network::Node>
    Node *get_ending_node() const;

  private:
    /// \brief gets the pointer out of the already constructed node vector.
    static Node *get_node_from_json(
        int direction, nlohmann::json const &edge_json,
        std::vector<std::unique_ptr<Node>> &nodes);

    Node *starting_node{nullptr};
    Node *ending_node{nullptr};
  };
} // namespace Network
