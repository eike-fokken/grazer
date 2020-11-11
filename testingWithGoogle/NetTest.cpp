#include "Net.hpp"
#include "Edge.hpp"
#include "MockSubproblem.hpp"
#include "Boundaryvalue.hpp"
#include "Node.hpp"
#include "Problem.hpp"
#include <Exception.hpp>
#include <algorithm>
#include <gtest/gtest.h>

struct NetTest : public ::testing ::Test {
  virtual void SetUp() override {}
  virtual void TearDown() override {}
};

TEST(testNet, test_NewNode_MakeEdgeBetween_ExistsEdgeBetween) {

  Network::Net net;

  for (unsigned int i = 0; i != 5; ++i) {
    net.new_node();
  }

  net.make_edge_between(0, 3);
  bool u = net.exists_edge_between(0, 3);
  bool v = net.exists_edge_between(0, 4);

  EXPECT_EQ(u, true);
  EXPECT_EQ(v, false);
}

TEST(testNet, test_RemoveEdgeBetween) {
  Network::Net net;

  net.new_node();
  net.new_node();

  net.make_edge_between(0, 1);
  net.remove_edge_between(0, 1);

  bool u = net.exists_edge_between(0, 1);
  auto nodes = net.get_nodes();

  int count(0);
  for (auto &node : nodes) {
    count +=
        node->get_starting_edges().size() + node->get_ending_edges().size();
  }
  EXPECT_EQ(count, 0);
  EXPECT_EQ(u, false);
}

TEST(testNet, test_get_valid_node_ids) {

  Network::Net net;

  net.new_node();
  net.new_node();

  auto vect = net.get_valid_node_ids();

  std::vector<int> BeautifulArray = {0, 1};

  EXPECT_EQ(vect, BeautifulArray);
}

TEST(testNet, test_RemoveNode) {
  Network::Net net;

  net.new_node();
  net.new_node();
  net.new_node();

  net.remove_node(1);

  auto vect = net.get_valid_node_ids();
  std::vector<int> BeautifulArray = {0, 2};

  EXPECT_EQ(vect, BeautifulArray);
}

TEST(testNode, test_GetId) {

  Network::Net net;
  net.new_node();
  int id = net.new_node();

  auto node1_ptr = net.get_node_by_id(id);

  int v = node1_ptr->get_id();

  EXPECT_EQ(v, 1);
}

TEST(testNode, test_HasId) {

  Network::Net net;
  int id = net.new_node();
  auto node1_ptr = net.get_node_by_id(id);

  bool v = node1_ptr->has_id(0);
  bool w = node1_ptr->has_id(1);

  EXPECT_EQ(v, true);
  EXPECT_EQ(w, false);
}

TEST(testNode, test_attachStartingEdge_getStartingEdges) {

  Network::Net net;
  int id0 = net.new_node();
  int id1 = net.new_node();

  auto node0_ptr = net.get_node_by_id(id0);
  auto node1_ptr = net.get_node_by_id(id1);

  net.make_edge_between(0, 1);
  auto edge_ptr = net.get_edge_by_node_ids(0, 1);

  node0_ptr->attach_starting_edge(edge_ptr);
  std::weak_ptr<Network::Edge> starting_edge_ptr =
      node0_ptr->get_starting_edges()[0];

  EXPECT_EQ(starting_edge_ptr.lock(), edge_ptr);
}

TEST(testNode, test_attachEndingEdge_getEndingEdges) {

  Network::Net net;
  int id0 = net.new_node();
  int id1 = net.new_node();

  auto node0_ptr = net.get_node_by_id(id0);
  auto node1_ptr = net.get_node_by_id(id1);

  net.make_edge_between(0, 1);
  auto edge_ptr = net.get_edge_by_node_ids(0, 1);

  node0_ptr->attach_ending_edge(edge_ptr);
  std::weak_ptr<Network::Edge> ending_edge_ptr =
      node0_ptr->get_ending_edges()[0];

  EXPECT_EQ(ending_edge_ptr.lock(), edge_ptr);
}

// hat Eike implementiert, etwas stimmt hier nicht
TEST(testEdge, test_removeEdge) {

  Network::Net net;
  int id0 = net.new_node();
  int id1 = net.new_node();

  auto node0_ptr = net.get_node_by_id(id0);
  auto node1_ptr = net.get_node_by_id(id1);

  net.make_edge_between(0, 1);
  auto edge_ptr = net.get_edge_by_node_ids(0, 1);

  node0_ptr->attach_starting_edge(edge_ptr);
  node0_ptr->remove_edge(
      edge_ptr); // da wird nichts removed, Befehl funktioniert nicht

  std::weak_ptr<Network::Edge> starting_edge_ptr =
      node0_ptr->get_starting_edges()[0];

  EXPECT_EQ(starting_edge_ptr.lock(), edge_ptr);

  //    Network::Node node4(4);
  //    Network::Node node3(3);
  //    auto node4_ptr = std::make_shared<Network::Node>(node4);
  //    auto node3_ptr = std::make_shared<Network::Node>(node3);

  //    Network::Edge edge1(node3_ptr, node4_ptr);
  //    auto edge1_ptr = std::make_shared<Network::Edge>(edge1);
  //    node3.attach_starting_edge(edge1_ptr);
  //    node3.remove_edge(edge1_ptr);

  //    //std::weak_ptr<Network::Edge> weak_edge1_ptr =
  //    node3.get_starting_edges()[0];

  //    EXPECT_EQ(1,1);
}

TEST(testEdge, test_getStartingNode) {

  Network::Net net;
  int id0 = net.new_node(); // das braucht man, sonst gibt es keinen node dafür
  int id1 = net.new_node();

  auto node0_ptr = net.get_node_by_id(id0);
  auto node1_ptr = net.get_node_by_id(id1);

  net.make_edge_between(1, 0);
  auto edge_ptr = net.get_edge_by_node_ids(1, 0);

  int start_edge1_id = edge_ptr->get_starting_node()->get_id();

  EXPECT_EQ(start_edge1_id, 1);
}

TEST(testEdge, test_getEndingNode) {

  Network::Net net;
  int id0 = net.new_node(); // das braucht man, sonst gibt es keinen node dafür
  int id1 = net.new_node();

  auto node0_ptr = net.get_node_by_id(id0);
  auto node1_ptr = net.get_node_by_id(id1);

  net.make_edge_between(1, 0);
  auto edge_ptr = net.get_edge_by_node_ids(1, 0);

  int ending_edge1_id = edge_ptr->get_ending_node()->get_id();

  EXPECT_EQ(ending_edge1_id, 0);
}

TEST(modelTest, get_number_of_states) {

  // Test how often this class is being called
  Model::MockSubproblem mocksub;
  EXPECT_CALL(mocksub, reserve_indices(0)).Times(1);

  mocksub.set_indices(0);
}

TEST(modelSubproblem, Model_evaluate) {

  Model::Problem problem(1);

  // make unique pointer of mocksub1 and mocksub2
  auto mock1_ptr = std::make_unique<Model::MockSubproblem>();
  auto mock2_ptr = std::make_unique<Model::MockSubproblem>();

  // add subproblem to problem
  problem.add_subproblem(std::move(mock1_ptr));
  problem.add_subproblem(std::move(mock2_ptr));

  // call problem.evaluate
  double last_time(0.0);
  double new_time(1.0);
  Eigen::VectorXd rootfunction(2);
  Eigen::VectorXd v1(2);
  v1(0) = 2;
  v1(1) = 3;
  Eigen::VectorXd v2(2);
  v1(0) = 3;
  v1(1) = 4;

  // expect the call to evaluate on the subproblems.
  // The cast magic is necessary to have the right type at hand...
  EXPECT_CALL(
      *dynamic_cast<Model::MockSubproblem *>(problem.get_subproblems()[0]),
      evaluate(rootfunction, last_time, new_time, v1, v2))
      .Times(1);
  EXPECT_CALL(
      *dynamic_cast<Model::MockSubproblem *>(problem.get_subproblems()[1]),
      evaluate(rootfunction, last_time, new_time, v1, v2))
      .Times(1);

  problem.evaluate(rootfunction, last_time, new_time, v1, v2);

}

TEST(Boundaryvalue,Operator) {

  //ARRANGE
  typedef Eigen::Matrix<double,2,1> Matrix2f; //2x1 matrix of double

  Matrix2f a;
  a(0,0)=0.0;
  a(1,0)=1.0;

  Matrix2f b;
  b(0,0)=1.0;
  b(1,0)=2.0;

  Matrix2f c;
  c(0,0)=2.0;
  c(1,0)=3.0;

  std::map<double, Eigen::Matrix<double, 2, 1>> MapOfBdrValues;
  MapOfBdrValues.insert(std::make_pair(1.0,a));
  MapOfBdrValues.insert(std::make_pair(2.0,b));
  MapOfBdrValues.insert(std::make_pair(3.0,c));

  Model::Networkproblem::Boundaryvalue <double,2> myBdrclass(MapOfBdrValues);
  
  //TEST1

  //ASSERT & ACT
  //Throw exception (der gleiche Test für zB 0.5 funktioniert nicht, ich weiß nicht warum. 
  //Vllt habe ich etwas missverstanden)
  EXPECT_ANY_THROW(myBdrclass.operator()(3.5));


  //TEST2

  //ASSERT
  //Test that returns value of interpolation for specific time step
  Eigen::VectorXd v2(2);
  Eigen::VectorXd v(2);
  v = myBdrclass.operator()(2.5);
  v2(0)=1.5;
  v2(1)=2.5;
  //ACT
  EXPECT_EQ(v, v2);
} 