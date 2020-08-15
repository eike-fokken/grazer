// #include<iostream>
// #include "Net.hpp"
// #include "Node.hpp"
// #include "Edge.hpp"

// int main()
 //{

//   Network::Net net;


//   for(int i=0;i!=5;++i)
//     {
//       auto a = net.new_node();
//       std::cout << "created node has id: " << a << std::endl;
//     }

//   net.make_edge_between(0, 3);
//   net.make_edge_between(0, 3);
//   net.make_edge_between(0, 4);
//   net.make_edge_between(1, 4);
//   net.make_edge_between(4, 2);

//   auto u = net.exists_edge_between(0,1);
//   std::cout << "this edge exists:: " << u << std::endl;

//   auto v = net.exists_edge_between(0,3);
//   std::cout << "this edge exists:: " << v << std::endl;

//   auto w = net.exists_edge_between(3,0);
//   std::cout << "this edge exists:: " << w << std::endl;
  
    
// //    Erweiterter Test von K.
//   Network::Node node4(4);
    
// //    node has id 4
//   auto x = node4.get_id();
//   std::cout << "the node's id is: " << x << std::endl;

//   auto y = node4.has_id(3);
//   std::cout << "Has node id 3?: " << y << std::endl;
  
//   auto z = node4.has_id(4);
//   std::cout << "Has node id 4?: " << z << std::endl;
 
// //attach_starting & ending edge
    
//   Network::Node node3(3);
//   auto node4_ptr = std::make_shared<Network::Node>(node4); //mache es zum shared_ptr
//   auto node3_ptr = std::make_shared<Network::Node>(node3); //mache es zum shared_ptr
//   Network::Edge edge1(node4_ptr, node3_ptr); //gerichtete Kante: 4->3
//   auto edge1_ptr = std::make_shared<Network::Edge>(edge1); //mache es zum shared_ptr
    
//   node4.attach_starting_edge(edge1_ptr); //node4 ist starting_edge
//   node3.attach_ending_edge(edge1_ptr); //node3 ist ending_edge
    
//  //exisitiert Kante 4->3 ?
//   auto b = net.exists_edge_between(4,3);
//   std::cout << "this edge exists: " << b << std::endl; //? klappt nicht so ganz
    
//   //schaue start und end Knoten von Edge1 an
//   int start_edge1_id = edge1_ptr -> get_starting_node() -> get_id();
//   int end_edge1_id = edge1_ptr -> get_ending_node() -> get_id();
    
//   std::cout << "get starting node of edge1: " << start_edge1_id << std::endl;
//   std::cout << "get ending node of edge1: " << end_edge1_id << std::endl;
    
//   //schaue starting_edges und ending_edges von Node4 an
//   /*std::vector<std::weak_ptr<Network::Edge>> g1 = node4_ptr->starting_edges; //iwas stimmt hier nicht
  
//   for (auto it = g1.begin(); it != g1.end(); it++){
//       //it is ein pointer vom Typ Network::Edge, mit .lock() machen wir daraus shared_ptr und können auf diesen zugreifen
//       int s = (*it).lock() -> get_starting_node() -> get_id();
//       std::cout << "( " << s;
//       int e = (*it).lock() -> get_ending_node() -> get_id();
//       std::cout << " )" << e;
//   }*/
// }
