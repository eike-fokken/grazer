#include "Edge.hpp"
#include "Node.hpp"
#include <memory>
#include <utility> //braucht man für Zeile 20
#include <vector>

namespace Network {

/// The class Net contains functions for creating nodes, connecting edges
/// and also removing both.

class Net {

public:
  /// This function creates a new node.
  /// It gives the new node simply an id equal to the number of the previosly
  /// exiting nodes in the net.
  /// @returns Id of new node
  int new_node();

  /// This function creates an edge between two nodes by id. Mind the order!
  /// Function throws error if an edge already exists.
  /// @param start Starting node of edge
  /// @param end Ending node of edge
  void make_edge_between(int start, int end);

  /// This function removes an edge between two nodes by id
  /// by going through a vector of all existing edges. Mind the order!
  /// @param node_1 Starting node of edge
  /// @param node_2 Ending node of edge
  void remove_edge_between(int node_1, int node_2);

  /// This function removes a node from the net by id by going through a vector
  /// of exiting nodes.
  /// @param node_id Node to remove
  void remove_node(int node_id);

  /// This function returns a vector of ids of all existing nodes in the net.
  /// @returns Vector of ids of the existing nodes
  std::vector<int> get_valid_node_ids();

  /// This function indicates whether an edge between two nodes exists or not.
  /// The order of nodes in the edge is not relevant. Its reverse edge is thus
  /// also checked.
  /// @param node_id_1 Starting Node of edge
  /// @param node_id_2 Ending Node of edge
  /// @returns Return True if the given edge or its reverse edge exists and
  /// false if neither exist.
  bool exists_edge_between(int node_id_1, int node_id_2) const;

  /// This function returns (by providing the nodes id) a pointer of type
  /// <Network::Node>.
  /// @param id Nodes id
  /// @returns Returns shared pointer of type <Network::Node>
  std::shared_ptr<Network::Node> get_node_by_id(int id);

  /// Similiar to Network::Net:get_node_by_id(int) this function returns
  /// a shared pointer of type <Network::Edge> given the corresponding nodes id.
  /// Mind the order!
  /// @param node_id_1 Nodes id of starting node
  /// @param node_id_2 Nodes id of ending node
  /// @returns Returns shared pointer of type < Network::Edge >
  std::shared_ptr<Network::Edge> get_edge_by_node_ids(int node_id_1,
                                                      int node_id_2);

  // std::set< std::pair<int,int > > get_edges() const;

private:
  std::vector<std::shared_ptr<Network::Node>> nodes;
  std::vector<std::shared_ptr<Network::Edge>> edges;
};

} // namespace Network
