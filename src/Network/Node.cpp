#include "../../include/Network/Node.hpp"
#include<memory>
#inlcude<set>
#include <stdexcept>


void Network::Node::remove_edge(std::weak_ptr<Network::Edge> to_remove)
    {
      auto no_starting_edge = starting_edges.erase(to_remove)
        if(no_starting_edge)
          {
           auto no_ending_edge = ending_edges.erase(to_remove)
           if(no_ending_edge)
             throw std::invalid_argument( "The edge is not attatched to the node" );
          }

    }



void Network::Node::attach_starting_edge(std::weak_ptr<Network::Edge> to_attach)
{
  //to be implemented
}

void Network::Node::attach_ending_edge(std::weak_ptr<Network::Edge> to_attach)
{
  //to be implemented
}
