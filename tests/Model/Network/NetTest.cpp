#include "Net.hpp"
#include "Edge.hpp"
#include "Node.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

class NodeTEST : public ::testing::Test {

public:
  NodeTEST() : node(R"({"id":"N1"})"_json){};

  Network::Node node;
};

class Nettest : public ::testing::Test {

public:
  Nettest() :
      net(std::vector<std::unique_ptr<Network::Node>>(),
          std::vector<std::unique_ptr<Network::Edge>>()) {
    std::vector<std::unique_ptr<Network::Node>> nodes;
    for (unsigned int i = 0; i != 5; ++i) {
      std::string s("N");
      s.append(std::to_string(i));
      nlohmann::json j;
      j["id"] = s;
      nodes.push_back(std::make_unique<Network::Node>(j));
    }

    std::vector<std::unique_ptr<Network::Edge>> edges;
    for (unsigned int i = 1; i != 3; ++i) {
      std::string s("E");
      std::string ns("N");
      s.append(std::to_string(i));
      s.append(std::to_string(0));
      ns.append(std::to_string(i));
      nlohmann::json j;
      j["id"] = s;
      j["from"] = "N0";
      j["to"] = ns;
      auto e = std::make_unique<Network::Edge>(j, nodes);
      edges.push_back(std::move(e));
    }
    net = Network::Net(std::move(nodes), std::move(edges));
  }
  Network::Net net;
};

TEST(Node_basic, test_successful_construction) {

  Network::Node node(R"({"id":"N1"})"_json);

  EXPECT_EQ(node.get_id(), "N1");
  EXPECT_EQ(node.get_starting_edges(), std::vector<Network::Edge *>());
  EXPECT_EQ(node.get_ending_edges(), std::vector<Network::Edge *>());
}

TEST(Edge, test_succesful_construction) {

  std::vector<std::unique_ptr<Network::Node>> nodes;
  nodes.push_back(std::make_unique<Network::Node>(R"({"id":"N1"})"_json));
  nodes.push_back(std::make_unique<Network::Node>(R"({"id":"N2"})"_json));

  auto edge_json = R"(
{
"id":"edgeid",
"from":"N1",
"to":"N2"
}
)"_json;
  Network::Edge edge(edge_json, nodes);
  EXPECT_EQ(edge.get_id(), "edgeid");
  EXPECT_EQ(edge.get_starting_node(), nodes[0].get());
  EXPECT_EQ(edge.get_ending_node(), nodes[1].get());
}

TEST_F(Nettest, test_ExistsEdgeBetween) {

  bool existing_edge = net.exists_edge_between("N0", "N2");
  bool non_existing_edge = net.exists_edge_between("N0", "N4");

  EXPECT_EQ(existing_edge, true);
  EXPECT_EQ(non_existing_edge, false);
}

TEST_F(Nettest, test_get_valid_node_ids) {

  auto vect = net.get_valid_node_ids();

  std::vector<std::string> BeautifulArray;
  for (unsigned int i = 0; i != 5; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    BeautifulArray.push_back(s);
  }

  EXPECT_EQ(vect, BeautifulArray);
}

TEST_F(Nettest, test_exists_node) {

  nlohmann::json j;
  j["id"] = "not_in_there";
  auto not_in_net = std::make_unique<Network::Node>(j);

  Network::Node *existing_node_pointer = net.exists_node(net.get_nodes()[3]);
  Network::Node *not_in_net_pointer = net.exists_node(not_in_net.get());

  EXPECT_EQ(not_in_net_pointer, nullptr);
  EXPECT_NE(existing_node_pointer, nullptr);
}

TEST_F(NodeTEST, test_GetId) {

  auto v = node.get_id();

  EXPECT_EQ(v, "N1");
}

TEST_F(Nettest, test_get_starting_edges) {

  auto nodevector = net.get_nodes();
  auto edgevector = net.get_edges();
  auto startedges0 = nodevector[0]->get_starting_edges();
  auto startedges1 = nodevector[1]->get_starting_edges();

  std::vector<Network::Edge *> expected_start_edges0;
  std::vector<Network::Edge *> expected_start_edges1;

  // All edges start at N0!
  for (unsigned int i = 0; i != 2; ++i) {
    expected_start_edges0.push_back(edgevector[i]);
  }

  EXPECT_EQ(startedges0, expected_start_edges0);
  EXPECT_EQ(startedges1, expected_start_edges1);
}

TEST_F(Nettest, getEndingEdges) {

  auto nodevector = net.get_nodes();
  auto edgevector = net.get_edges();

  auto endedges0 = nodevector[0]->get_ending_edges();
  auto endedges1 = nodevector[1]->get_ending_edges();

  std::vector<Network::Edge *> expected_end_edges;

  expected_end_edges.push_back(edgevector[0]);

  EXPECT_EQ(endedges0.size(), 0);
  EXPECT_EQ(endedges1, expected_end_edges);
}

TEST_F(Nettest, test_getStartingNode) {

  auto nodes = net.get_nodes();
  auto edges = net.get_edges();

  EXPECT_EQ(edges[0]->get_starting_node(), nodes[0]);
}

TEST_F(Nettest, test_getEndingNode) {

  auto nodes = net.get_nodes();
  auto edges = net.get_edges();

  EXPECT_EQ(edges[0]->get_ending_node(), nodes[1]);
}
