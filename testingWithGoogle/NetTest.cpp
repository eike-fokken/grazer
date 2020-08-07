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


TEST(testNetwork, test_MakeEdgeBetween_ExistsEdgeBetween) {
    
  Network::Net net;

  for(int i=0;i!=5;++i)
      {
        auto a = net.new_node();
      }

  net.make_edge_between(0,3);
  auto u = net.exists_edge_between(0,3);
  auto v = net.exists_edge_between(0,4);

  EXPECT_EQ(u,  1);
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

TEST(testNode, test_getStartingNode) {
    
  Network::Node node4(4);
  Network::Node node3(3);
  auto node4_ptr = std::make_shared<Network::Node>(node4);
  auto node3_ptr = std::make_shared<Network::Node>(node3);
  Network::Edge edge1(node4_ptr, node3_ptr);
  auto edge1_ptr = std::make_shared<Network::Edge>(edge1);

  int start_edge1_id = edge1_ptr -> get_starting_node() -> get_id();

  EXPECT_EQ(start_edge1_id,4);

}

TEST(testNode, test_getEndingNode) {
    
  Network::Node node4(4);  
  Network::Node node3(3);
  auto node4_ptr = std::make_shared<Network::Node>(node4);
  auto node3_ptr = std::make_shared<Network::Node>(node3);
  Network::Edge edge1(node4_ptr, node3_ptr);
  auto edge1_ptr = std::make_shared<Network::Edge>(edge1);

  int end_edge1_id = edge1_ptr -> get_ending_node() -> get_id();

  EXPECT_EQ(end_edge1_id,3);

}
