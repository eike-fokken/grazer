#include <gtest/gtest.h>
#include <iostream>
#include "Net.hpp"
#include "Node.hpp"
#include "Edge.hpp"

//löschen
struct NetTest: public :: testing :: Test {
    virtual void SetUp() override {
        
    }
    virtual void TearDown() override {}
};


TEST(testNetwork, testMakeEdgeBetween) {
    
Network::Net net;

for(int i=0;i!=5;++i)
    {
      auto a = net.new_node();
    }

net.make_edge_between(0,3);
auto u = net.exists_edge_between(0,3);

EXPECT_EQ (u,  1);

}

//verbessern variablen
TEST(NodeTest2, NodeTest2) {
    
Network::Net net;

for(int i=0;i!=5;++i)
    {
      auto a = net.new_node();
    }

net.make_edge_between(0,3);
auto v = net.exists_edge_between(0,1);

EXPECT_EQ (v,  0);

}