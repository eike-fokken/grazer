#inlcude<vector>
#inlcude<set>
#inlcude<utility>
#include "Edge.hpp"
#include "Node.hpp"

namespace Network {

class Net
{


  Net(): highest_ever_used_id(0) {};

  ~Net();

  int new_node();
  void make_edge_between(int start,int end);
  void remove_edge_between(int node_1,int node_2);
  void remove_node(int node_id);

  std::set<int> get_valid_node_ids();
  bool exists_edge_between(int node_id_1, int node_id_2);
  std::set< std::pair<int,int > > get_connections();


private:
  int highest_ever_used_id;
  std::vector< std::shared_ptr< Network::Node > > nodes;
  std::vector< std::shared_ptr< Network::Edge > > edges;



};


}
