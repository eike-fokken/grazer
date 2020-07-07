#include "../../include/Network/Net.hpp"

bool has_id(const int id, const Network::Node& object)
{ return obj.getid() == id; }

bool Network::Net::exists_edge_between(int node_id_1, int node_id_2)
{
auto iterator = std::find_if (nodes.begin(), nodes.end(), IsOdd);


}

void Network::Net::make_edge_between(int start,int end)
{

  std::make_shared<Network::Edge> new_edge(start,end);
  edges.push_back(new_edge);
  start.attach_starting_edge(new_edge);
  end.attach_ending_edge(new_edge);
}

int Network::Net::new_node()
{
  id=highest_ever_used_id;
  ++highest_ever_used_id;
  std::make_shared<Network::Node> new_node(id);
  nodes.push_back(new_node);
  return id;
}
