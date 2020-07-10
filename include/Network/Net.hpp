#include<vector>
#include<utility>
#include "Edge.hpp"
#include "Node.hpp"

namespace Network {

class Net
{

public:

  int new_node();
  void make_edge_between(int start,int end);
  void remove_edge_between(int node_1,int node_2);
  void remove_node(int node_id);

  std::vector<int> get_valid_node_ids() const;
  bool exists_edge_between(int node_id_1, int node_id_2) const;
  //std::set< std::pair<int,int > > get_edges() const;


private:
  std::vector< std::shared_ptr< Network::Node > > nodes;
  std::vector< std::shared_ptr< Network::Edge > > edges;

};


}
