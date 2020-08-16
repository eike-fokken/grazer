#include<vector>
#include<memory>
#include<utility> //braucht man für Zeile 20
#include "Edge.hpp"
#include "Node.hpp"

namespace Network {

class Net
{

public:

  int new_node();
  void make_edge_between(int start,int end); 
  void remove_edge_between(int node_1,int node_2); //implementiert & getestet
  void remove_node(int node_id); // implementiert & getestet

  std::vector<int> get_valid_node_ids(); //implementiert & getestet
  bool exists_edge_between(int node_id_1, int node_id_2) const;
  std::shared_ptr< Network::Node > get_node_by_id(int id);
  std::shared_ptr< Network::Edge > get_edge_by_node_ids(int node_id_1, int node_id_2);
  //std::set< std::pair<int,int > > get_edges() const;


private:
  std::vector< std::shared_ptr< Network::Node > > nodes;
  std::vector< std::shared_ptr< Network::Edge > > edges;

};


}
