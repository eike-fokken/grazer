#include "Net.hpp"
#include "Boundaryvalue.hpp"
#include "Edge.hpp"
#include "MockSubproblem.hpp"
#include "Node.hpp"
#include "Problem.hpp"
#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

struct NetTest : public ::testing ::Test {
  virtual void SetUp() override {}
  virtual void TearDown() override {}
};

TEST(testNet, test_NewNode_MakeEdgeBetween_ExistsEdgeBetween) {

  std::vector<std::unique_ptr<Network::Node>> nodes;
  for (unsigned int i = 0; i != 5; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    nodes.push_back(std::unique_ptr<Network::Node>(new Network::Node(s)));
  }

  std::vector<std::unique_ptr<Network::Edge>> edges;
  auto E1 = std::unique_ptr<Network::Edge>(
      new Network::Edge("edge", nodes[0].get(), nodes[3].get()));
  edges.push_back(std::move(E1));
  Network::Net net(std::move(nodes), std::move(edges));

  bool u = net.exists_edge_between("N0", "N3");
  bool v = net.exists_edge_between("N0", "N4");

  EXPECT_EQ(u, true);
  EXPECT_EQ(v, false);
}

TEST(testNet, test_get_valid_node_ids) {
  std::vector<std::unique_ptr<Network::Node>> nodes;
  for (unsigned int i = 0; i != 5; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    nodes.push_back(std::unique_ptr<Network::Node>(new Network::Node(s)));
  }

  std::vector<std::unique_ptr<Network::Edge>> edges;
  auto E1 = std::unique_ptr<Network::Edge>(
      new Network::Edge("edge", nodes[0].get(), nodes[3].get()));
  edges.push_back(std::move(E1));

  Network::Net net(std::move(nodes), std::move(edges));

  auto vect = net.get_valid_node_ids();
  std::vector<std::string> BeautifulArray;
  for (unsigned int i = 0; i != 5; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    BeautifulArray.push_back(s);
  }

  EXPECT_EQ(vect, BeautifulArray);
}

TEST(testNode, test_GetId) {
  std::vector<std::unique_ptr<Network::Node>> nodes;
  for (unsigned int i = 0; i != 5; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    nodes.push_back(std::unique_ptr<Network::Node>(new Network::Node(s)));
  }

  std::vector<std::unique_ptr<Network::Edge>> edges;
  auto E1 = std::unique_ptr<Network::Edge>(
      new Network::Edge("edge", nodes[0].get(), nodes[3].get()));
  edges.push_back(std::move(E1));
  Network::Net net(std::move(nodes), std::move(edges));
  auto node1_ptr = net.get_node_by_id("N3");

  auto v = node1_ptr->get_id();

  EXPECT_EQ(v, "N3");
}

TEST(testNode, test_exists_node) {
  std::vector<std::unique_ptr<Network::Node>> nodes;
  for (unsigned int i = 0; i != 5; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    nodes.push_back(std::unique_ptr<Network::Node>(new Network::Node(s)));
  }

  std::vector<std::unique_ptr<Network::Edge>> edges;
  auto E1 = std::unique_ptr<Network::Edge>(
      new Network::Edge("edge", nodes[0].get(), nodes[3].get()));
  edges.push_back(std::move(E1));
  Network::Net net(std::move(nodes), std::move(edges));

  Network::Node *e = net.exists_node(net.get_nodes()[3]);
  auto not_in_there = std::unique_ptr<Network::Node>(new Network::Node("haha"));

  Network::Node *n = net.exists_node(not_in_there.get());

  EXPECT_EQ(n, nullptr);
  EXPECT_NE(e, nullptr);
}

TEST(testNode, test_get_starting_edges) {

  std::vector<std::unique_ptr<Network::Node>> nodes;
  for (unsigned int i = 0; i != 5; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    nodes.push_back(std::unique_ptr<Network::Node>(new Network::Node(s)));
  }

  std::vector<std::unique_ptr<Network::Edge>> edges;
  edges.push_back(std::unique_ptr<Network::Edge>(
      new Network::Edge("E34", nodes[3].get(), nodes[4].get())));

  for (unsigned int i = 1; i != 3; ++i) {
    std::string s("E");
    s.append(std::to_string(i));
    s.append(std::to_string(0));
    auto e = std::unique_ptr<Network::Edge>(
        new Network::Edge(s, nodes[0].get(), nodes[i].get()));
    edges.push_back(std::move(e));
  }

  auto startedges = nodes[0]->get_starting_edges();

  std::vector<Network::Edge *> expected_start_edges;

  for (unsigned int i = 1; i != 3; ++i) {
    expected_start_edges.push_back(edges[i].get());
  }

  auto endedges = nodes[0]->get_ending_edges();

  EXPECT_EQ(endedges.size(), 0);
  EXPECT_EQ(startedges, expected_start_edges);
}

TEST(testNode, test_attachEndingEdge_getEndingEdges) {

  std::vector<std::unique_ptr<Network::Node>> nodes;
  for (unsigned int i = 0; i != 5; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    nodes.push_back(std::unique_ptr<Network::Node>(new Network::Node(s)));
  }

  std::vector<std::unique_ptr<Network::Edge>> edges;
  edges.push_back(std::unique_ptr<Network::Edge>(
      new Network::Edge("E34", nodes[3].get(), nodes[4].get())));

  for (unsigned int i = 1; i != 3; ++i) {
    std::string s("E");
    s.append(std::to_string(i));
    s.append(std::to_string(0));
    auto e = std::unique_ptr<Network::Edge>(
        new Network::Edge(s, nodes[i].get(), nodes[0].get()));
    edges.push_back(std::move(e));
  }

  auto endedges = nodes[0]->get_ending_edges();

  std::vector<Network::Edge *> expected_end_edges;

  for (unsigned int i = 1; i != 3; ++i) {
    expected_end_edges.push_back(edges[i].get());
  }

  auto startedges = nodes[0]->get_starting_edges();

  EXPECT_EQ(startedges.size(), 0);
  EXPECT_EQ(endedges, expected_end_edges);
}

TEST(testEdge, test_getStartingNode) {

  std::vector<std::unique_ptr<Network::Node>> nodes;
  for (unsigned int i = 0; i != 2; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    nodes.push_back(std::unique_ptr<Network::Node>(new Network::Node(s)));
  }

  auto edge = std::unique_ptr<Network::Edge>(
      new Network::Edge("E01", nodes[0].get(), nodes[1].get()));

  EXPECT_EQ(edge->get_starting_node(), nodes[0].get());
}

TEST(testEdge, test_getEndingNode) {

  std::vector<std::unique_ptr<Network::Node>> nodes;
  for (unsigned int i = 0; i != 2; ++i) {
    std::string s("N");
    s.append(std::to_string(i));
    nodes.push_back(std::unique_ptr<Network::Node>(new Network::Node(s)));
  }

  auto edge = std::unique_ptr<Network::Edge>(
      new Network::Edge("E10", nodes[1].get(), nodes[0].get()));

  EXPECT_EQ(edge->get_ending_node(), nodes[0].get());
}

TEST(modelTest, get_number_of_states) {

  // Test how often this class is being called
  GrazerTest::MockSubproblem mocksub;
  EXPECT_CALL(mocksub, reserve_indices(0)).Times(1);

  mocksub.set_indices(0);
}

TEST(modelSubproblem, Model_evaluate) {

  Model::Problem problem("");

  // make unique pointer of mocksub1 and mocksub2
  auto mock1_ptr = std::make_unique<GrazerTest::MockSubproblem>();
  auto mock2_ptr = std::make_unique<GrazerTest::MockSubproblem>();

  // add subproblem to problem
  problem.add_subproblem(std::move(mock1_ptr));
  problem.add_subproblem(std::move(mock2_ptr));

  // call problem.evaluate
  double last_time(0.0);
  double new_time(1.0);
  Eigen::VectorXd rootvalues(2);
  Eigen::VectorXd v1(2);
  v1(0) = 2;
  v1(1) = 3;
  Eigen::VectorXd v2(2);
  v1(0) = 3;
  v1(1) = 4;

  // This is necessary for the expect_call to work properly...
  Eigen::Ref<Eigen::VectorXd> rootref(rootvalues);

  //  // expect the call to evaluate on the subproblems.
  //  // The cast magic is necessary to have the right type at hand...
  EXPECT_CALL(
      *dynamic_cast<GrazerTest::MockSubproblem *>(problem.get_subproblems()[0]),
      evaluate(rootref, last_time, new_time, v1, v2))
      .Times(1);
  EXPECT_CALL(
      *dynamic_cast<GrazerTest::MockSubproblem *>(problem.get_subproblems()[1]),
      evaluate(rootref, last_time, new_time, v1, v2))
      .Times(1);

  problem.evaluate(rootref, last_time, new_time, v1, v2);
}

TEST(Boundaryvalue, Operator) {

  // ARRANGE
  typedef Eigen::Matrix<double, 2, 1> Matrix2f; // 2x1 matrix of double

  Matrix2f a;
  a(0, 0) = 0.0;
  a(1, 0) = 1.0;

  Matrix2f b;
  b(0, 0) = 1.0;
  b(1, 0) = 2.0;

  Matrix2f c;
  c(0, 0) = 2.0;
  c(1, 0) = 3.0;

  std::map<double, Eigen::Matrix<double, 2, 1>> MapOfBdrValues;
  MapOfBdrValues.insert(std::make_pair(1.0, a));
  MapOfBdrValues.insert(std::make_pair(2.0, b));
  MapOfBdrValues.insert(std::make_pair(3.0, c));

  Model::Networkproblem::Boundaryvalue<double, 2> myBdrclass(MapOfBdrValues);

  EXPECT_ANY_THROW(myBdrclass.operator()(3.5));
  try {
    myBdrclass(3.5);
  } catch (Exception &e) {
    std::string message(e.what());
    bool is_right_message =
        (message.find("Requested boundary value is at a later time than the "
                      "given values") != std::string::npos);
    EXPECT_EQ(is_right_message, true);
  }
  EXPECT_ANY_THROW(myBdrclass.operator()(0.5));

  // TEST2
  {
    Eigen::VectorXd v2(2);
    Eigen::VectorXd v(2);
    v = myBdrclass.operator()(1.0);
    v2 = a;
    // ACT
    EXPECT_EQ(v, v2);
  }

  {
    Eigen::VectorXd v2(2);
    Eigen::VectorXd v(2);
    v = myBdrclass.operator()(3.0);
    v2 = c;
    // ACT
    EXPECT_EQ(v, v2);
  }

  // ASSERT
  // Test that returns value of interpolation for specific time step
  {
    Eigen::VectorXd v2(2);
    Eigen::VectorXd v(2);
    v = myBdrclass.operator()(2.5);
    v2 = 0.5 * (b + c);
    // ACT
    EXPECT_EQ(v, v2);
  }
}
