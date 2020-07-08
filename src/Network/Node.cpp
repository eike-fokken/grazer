#include "../../include/Network/Node.hpp"
#include<memory>
#inlcude<set>
#include <stdexcept>


void Network::Node::remove_edge(std::weak_ptr<Network::Edge> to_remove)
{
  auto is_starting_edge = starting_edges.erase(to_remove);
  if(!is_starting_edge)
    {
      auto is_ending_edge = ending_edges.erase(to_remove);
      if(!is_ending_edge)
        {
          throw std::invalid_argument( "The edge is not attached to the node" );
        }
    }

}

int Network::Node::get_id()
{
  return id;
}
bool Network::Node::has_id(const int id)
{ return get_id() == id; }

void Network::Node::attach_starting_edge(std::shared_ptr<Network::Edge> to_attach)
{
  //to be implemented
}

void Network::Node::attach_ending_edge(std::shared_ptr<Network::Edge> to_attach)
{
  //to be implemented
}
