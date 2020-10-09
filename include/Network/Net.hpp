#include<vector>
#include<memory>
#include<utility> //braucht man für Zeile 20
#include "Edge.hpp"
#include "Node.hpp"

namespace Network {

///The class net contains all relevant functions for its construction.

class Net
{

public:

  /// Function creates a new node. Starts in chronological order.
  /// Gives the new node simply an id equal to the number of the previosly exiting nodes.
  int new_node();


  /// This function creates an edge between two nodes. Mind the order!
  /// Throws error if an edge already exists.
  /// @param start Starting Node of edge
  /// @param end Ending Node of edge
  void make_edge_between(int start,int end); 


  /// This function removes an edge between two nodes 
  /// by going through vector of existing edges. Mind the order!
  /// @param start Starting Node of edge
  /// @param end Ending Node of edge
  void remove_edge_between(int node_1,int node_2);


  /// Removes node from the net, by going through vector of  exiting nodes.
  /// @param node_id Node to remove
  void remove_node(int node_id); 


  /// Returns vector with all existing nodes in the net.
  /// @returns Return is a vector with existing nodes
  std::vector<int> get_valid_node_ids(); 
  

  /// This function proofs if an edge between two nodes exitst.
  /// The order of nodes in the edge does not play a role.
  /// @param node_id_1 Starting Node of edge
  /// @param node_id_2 Ending Node of edge
  /// @returns Return True if this edge or its reverse edge exists, and false if neither exist
  bool exists_edge_between(int node_id_1, int node_id_2) const;


  /// The function returns shared pointer of type < Network::Node > given its id.
  /// @param id Id of desired node
  /// @returns Returns shared pointer of type < Network::Node > 
  std::shared_ptr< Network::Node > get_node_by_id(int id);


  /// The function return similiar to Network::Net:get_node_by_id(int) a
  /// shared pointer of type < Network::Edge > given the corresponding node ids.
  /// Mind the order!
  /// @param node_id_1 Node id of starting node
  /// @param node_id_2 Node id of ending node
  /// @returns Returns shared pointer of type < Network::Edge >
  std::shared_ptr< Network::Edge > get_edge_by_node_ids(int node_id_1, int node_id_2);
  
  
  
  //std::set< std::pair<int,int > > get_edges() const;


private:
  std::vector< std::shared_ptr< Network::Node > > nodes;
  std::vector< std::shared_ptr< Network::Edge > > edges;

};


}
