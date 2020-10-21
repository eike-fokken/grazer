#pragma once
#include <memory>
#include <vector>

namespace Network {

class Net;

class Edge;

/// The class node contains all relevant functions for handling attachments and
/// removement of edges as well as providing information about the nodes id.

class Node {
public:
  /// This function attaches an edge to object of type node.
  /// Attention: Here object node has to be the starting node in the edge you
  /// provide.
  /// @param to_attach Edge to attach
  void attach_starting_edge(std::shared_ptr<Network::Edge> to_attach);

  /// This function attaches an edge to object of type node.
  /// Attention: Here object node has to be the ending node in the edge you
  /// provide.
  /// @param to_attach Edge to attach
  void attach_ending_edge(std::shared_ptr<Network::Edge> to_attach);

  /// This function removes the edge to_remove from the object node.
  /// If such edge does not exist, it throws an error message.
  /// Therefore the function goes through all starting_edges and ending_edges
  /// (it does not remove the reverse one, if such exists).
  /// @param to_remove Edge that has to be removed
  void remove_edge(
      std::shared_ptr<Network::Edge> to_remove); // etwas ist hier falsch

  /// This function returns vector of weak pointer of starting edges
  /// @returns  std::vector<std::weak_ptr<Network::Edge> > starting_edges
  std::vector<std::weak_ptr<Network::Edge>> get_starting_edges() const;

  /// This function returns vector of weak pointer of ending edges
  /// @returns  std::vector<std::weak_ptr<Network::Edge> > ending_edges
  std::vector<std::weak_ptr<Network::Edge>> get_ending_edges() const;

  /// This function returns the id of object node
  /// @returns unsigned int
  unsigned int get_id() const;

  /// This function returns 1, if id is equal to nodes true id and 0 otherwise.
  /// @param id Possible nodes id
  /// @returns 1 (true) or 0 (false)
  bool has_id(const unsigned int id) const;

private:
  // Default constructor not allowed:
  Node() = delete;

  // Constructor must at least provide an id:k
  Node(unsigned int _id) : id(_id){};

  unsigned int id;
  std::vector<std::weak_ptr<Network::Edge>> starting_edges;
  std::vector<std::weak_ptr<Network::Edge>> ending_edges;

  friend class Network::Net;
};

} // namespace Network
