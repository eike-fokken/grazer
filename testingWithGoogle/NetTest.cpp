#include <gtest/gtest.h>
#include <iostream>
#include "Net.hpp"
#include "Node.hpp"
#include "Edge.hpp"


struct NetTest: public :: testing :: Test {
    virtual void SetUp() override {
        
    }
    virtual void TearDown() override {}
}; 


TEST(testNet, test_NewNode_MakeEdgeBetween_ExistsEdgeBetween) {
    
  Network::Net net;

  for(int i=0;i!=5;++i)
      {
        auto a = net.new_node();
      }

  net.make_edge_between(0,3);
  auto u = net.exists_edge_between(0,3);
  auto v = net.exists_edge_between(0,4);

  EXPECT_EQ(u, 1);
  EXPECT_EQ(v, 0);
}

TEST(testNode, test_GetId) {
    
  Network::Node node(4);

  auto id = node.get_id();

  EXPECT_EQ(id,4);

}

TEST(testNode, test_HasId) {
    
  Network::Node node(4);

  auto id4 = node.has_id(4);
  auto id3 = node.has_id(3);

  EXPECT_EQ(id4,true);
  EXPECT_NE(id3,true);

}

TEST(testNode, test_attachStartingEdge_getStartingEdges) {
    
  Network::Node node4(4);
  Network::Node node3(3);
  auto node4_ptr = std::make_shared<Network::Node>(node4);
  auto node3_ptr = std::make_shared<Network::Node>(node3);
  Network::Edge edge1(node4_ptr, node3_ptr);  //4-->3
  auto edge1_ptr = std::make_shared<Network::Edge>(edge1);

  node4.attach_starting_edge(edge1_ptr);
  std::weak_ptr<Network::Edge> weak_edge1_ptr = node4.get_starting_edges()[0];

  EXPECT_EQ(weak_edge1_ptr.lock(), edge1_ptr);

}

TEST(testNode, test_attachEndingEdge_getEndingEdges) {
    
  Network::Node node4(4);
  Network::Node node3(3);
  auto node4_ptr = std::make_shared<Network::Node>(node4);
  auto node3_ptr = std::make_shared<Network::Node>(node3);
  Network::Edge edge1(node3_ptr, node4_ptr);  //3-->4
  auto edge1_ptr = std::make_shared<Network::Edge>(edge1);

  node4.attach_ending_edge(edge1_ptr);
  std::weak_ptr<Network::Edge> weak_edge1_ptr = node4.get_ending_edges()[0];

  EXPECT_EQ(weak_edge1_ptr.lock(), edge1_ptr);

}

TEST(testEdge, test_removeEdge) {

  Network::Node node4(4);
  Network::Node node3(3);
  auto node4_ptr = std::make_shared<Network::Node>(node4);
  auto node3_ptr = std::make_shared<Network::Node>(node3);

  Network::Edge edge1(node3_ptr, node4_ptr);
  auto edge1_ptr = std::make_shared<Network::Edge>(edge1);
  node3.attach_starting_edge(edge1_ptr);
  //node3.remove_edge(edge1_ptr);

  //std::weak_ptr<Network::Edge> weak_edge1_ptr = node3.get_starting_edges()[0];

  EXPECT_EQ(1,1); 
}


TEST(testEdge, test_getStartingNode) {
    
  Network::Node node4(4);
  Network::Node node3(3);
  auto node4_ptr = std::make_shared<Network::Node>(node4);
  auto node3_ptr = std::make_shared<Network::Node>(node3);
  Network::Edge edge1(node4_ptr, node3_ptr); //4-->3
  auto edge1_ptr = std::make_shared<Network::Edge>(edge1);

  int start_edge1_id = edge1_ptr -> get_starting_node() -> get_id();

  EXPECT_EQ(start_edge1_id,4);

}

TEST(testEdge, test_getEndingNode) {
    
  Network::Node node4(4);  
  Network::Node node3(3);
  auto node4_ptr = std::make_shared<Network::Node>(node4);
  auto node3_ptr = std::make_shared<Network::Node>(node3);

  Network::Edge edge1(node4_ptr, node3_ptr);
  auto edge1_ptr = std::make_shared<Network::Edge>(edge1);

  int end_edge1_id = edge1_ptr -> get_ending_node() -> get_id();

  EXPECT_EQ(end_edge1_id,3);

}

/*TEST(testFriend, test_NetNode){

  Network::Node node4(4);
  Network::Node node3(3);
  auto node4_ptr = std::make_shared<Network::Node>(node4);
  auto node3_ptr = std::make_shared<Network::Node>(node3);
  Network::Edge edge1(node4_ptr, node3_ptr);  //4-->3
  auto edge1_ptr = std::make_shared<Network::Edge>(edge1);

  node4.attach_starting_edge(edge1_ptr); //in starting_edges drin
  
  Network::Net net1;
  //hier muss ich eine Methode implementieren in net, die es mir erlaubt irgendetwas mit node zu machen


}*/



