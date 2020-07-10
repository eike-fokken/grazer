#include<iostream>
#include "Net.hpp"
#include "Node.hpp"
#include "Edge.hpp"

int main()
{

  Network::Net net;


  for(int i=0;i!=5;++i)
    {
      auto a = net.new_node();
      std::cout << "created node has id: " << a << std::endl;
    }

  net.make_edge_between(0, 3);
  net.make_edge_between(0, 3);
  net.make_edge_between(0, 4);
  net.make_edge_between(1, 4);
  net.make_edge_between(4, 2);

  auto u = net.exists_edge_between(0,1);
  std::cout << "this edge exists:: " << u << std::endl;

  auto v = net.exists_edge_between(0,3);
  std::cout << "this edge exists:: " << v << std::endl;

  auto w = net.exists_edge_between(3,0);
  std::cout << "this edge exists:: " << w << std::endl;


}
