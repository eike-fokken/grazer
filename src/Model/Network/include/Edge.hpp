#pragma once
#include <Idobject.hpp>

namespace Network {

  // This is a "forward declaration", so that one can use the Node class here:
  class Node;

  class Net;

  /** @brief The class edge contains functions for getting information
   * about the nodes attached to object Edge.
   */
  class Edge : public Idobject {

  public:
    static nlohmann::json get_schema();

    // The edge holds a  pointer to its start and end nodes.
    // Creating an edge without also providing its start and
    // end node is prohibited:
    Edge() = delete;

    /** \brief Builds an edge from an appropriate json AND registers it in the
     * start and end nodes.
     */
    Edge(
        nlohmann::json const &edge_json,
        std::vector<std::unique_ptr<Node>> &nodes);

    virtual ~Edge();

    /**
     * @brief Function returns starting node of Edge object.
     *
     * @return Node*
     */
    Node *get_starting_node() const;

    /**
     * @brief Function returns ending node of Edge object.
     *
     * @return Node*
     */
    Node *get_ending_node() const;

  private:
    /**
     * @brief gets the pointer out of the already constructed node vector.
     *
     * @param    direction            TODO
     * @param    edge_json            TODO
     * @param    nodes                TODO
     * @return Node*
     */
    static Node *get_node_from_json(
        int direction, nlohmann::json const &edge_json,
        std::vector<std::unique_ptr<Node>> &nodes);

    Node *starting_node{nullptr};
    Node *ending_node{nullptr};
  };
} // namespace Network
