#include "Flowboundarynode.hpp"
#include "Gas_factory.hpp"
#include "Gaspowerconnection.hpp"
#include "Innode.hpp"
#include "Matrixhandler.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

struct Catch_cout {
  Catch_cout( std::streambuf * new_buffer )
        : old( std::cout.rdbuf( new_buffer ) )
    { }

    ~Catch_cout( ) {
        std::cout.rdbuf( old );
    }

private:
    std::streambuf * old;
};


namespace Eigen {
  template <typename Derived>
  void to_json(nlohmann::json &j, const MatrixBase<Derived> &eig);
}

template <typename Derived0, typename Derived1>
nlohmann::json
make_value_json(std::string id, std::string key, Eigen::MatrixBase<Derived0> const &condition0,
                Eigen::MatrixBase<Derived1> const &condition1);

nlohmann::json make_value_json(std::string id, std::string key,
                               double condition0, double condition1);


class GasTEST : public ::testing::Test {
public:
  std::string output;

  std::unique_ptr<Model::Networkproblem::Networkproblem>
  get_Networkproblem(nlohmann::json &netproblem) {
    std::unique_ptr<Model::Networkproblem::Networkproblem> netprob;
    {
      std::stringstream buffer;
      Catch_cout catcher(buffer.rdbuf());
      auto net_ptr = Model::Networkproblem::build_net<
          Model::Componentfactory::Gas_factory>(netproblem);
      netprob = std::make_unique<Model::Networkproblem::Networkproblem>(
          std::move(net_ptr));
      output = buffer.str();
    }

    return netprob;
  }
};

nlohmann::json source_json(std::string id, double flowstart,
                           double flowend);

nlohmann::json sink_json(std::string id , double flowstart,
                         double flowend);

nlohmann::json shortpipe_json(std::string id,nlohmann::json startnode,
                              nlohmann::json endnode);

// nlohmann::json pipe_json(nlohmann::json startnode, nlohmann::json endnode,
//                               std::string id = "pipe");

nlohmann::json make_full_json(std::vector<nlohmann::json> sources,
                              std::vector<nlohmann::json> sinks,
                              std::vector<nlohmann::json> innodes,
                              std::vector<nlohmann::json> shortpipes,
                              std::vector<nlohmann::json> pipes);

nlohmann::json make_initial_json(std::vector<nlohmann::json> shortpipes,
                                 std::vector<nlohmann::json> pipes);

TEST_F(GasTEST, Shortpipe_evaluate) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow1start = -23.0;
  double flow1end = -440.0;

  auto node0_topology = source_json("node0", flow0start, flow0end);
  auto node1_topology = source_json("node1", flow1start,flow1end);
  auto shortpipe_topology = shortpipe_json("shortpipe",node0_topology, node1_topology);
  auto np_json = make_full_json({node0_topology, node1_topology}, {}, {},
                                {shortpipe_topology}, {});

  auto netprob = get_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);

  
  // initial values:
  double pressure_start = 810;
  double flow_start = -4;
  double pressure_end = 125;
  double flow_end = 1000;
  Eigen::Vector2d cond0(pressure_start, flow_start);
  Eigen::Vector2d cond1(pressure_end, flow_end);

  auto initial_json = make_value_json("shortpipe", "x", cond0, cond1);

  nlohmann::json np_initialjson;
  np_initialjson["connections"]["Shortpipe"].push_back(initial_json);
  netprob->set_initial_values(last_state, np_initialjson);
  Eigen::VectorXd new_state = last_state;
  netprob->evaluate(rootvalues,  last_time, new_time, last_state, new_state);


  EXPECT_DOUBLE_EQ(rootvalues[1], pressure_start-pressure_end );
  EXPECT_DOUBLE_EQ(rootvalues[2], flow_start-flow_end );
}

TEST_F(GasTEST, Shortpipe_evaluate_state_derivative) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow1start = -23.0;
  double flow1end = -440.0;

  auto node0_topology = source_json("node0", flow0start, flow0end);
  auto node1_topology = source_json("node1", flow1start, flow1end);
  auto shortpipe_topology =
      shortpipe_json("shortpipe", node0_topology, node1_topology);
  auto np_json = make_full_json({node0_topology, node1_topology}, {}, {},
                                {shortpipe_topology}, {});

  auto netprob = get_Networkproblem(np_json);

  int number_of_variables = netprob->set_indices(0);

  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);

  // set initial values

  // initial values:
  double pressure_start = 810;
  double flow_start = -4;
  double pressure_end = 125;
  double flow_end = 1000;
  Eigen::Vector2d cond0(pressure_start, flow_start);
  Eigen::Vector2d cond1(pressure_end, flow_end);

  auto initial_json = make_value_json("shortpipe", "x", cond0, cond1);

  nlohmann::json np_initialjson;
  np_initialjson["connections"]["Shortpipe"].push_back(initial_json);
  netprob->set_initial_values(last_state, np_initialjson);
  Eigen::VectorXd new_state = last_state;
  netprob->evaluate(rootvalues,  last_time, new_time, last_state, new_state);

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  netprob->evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  handler.set_matrix();


  Eigen::Matrix4d DenseJ = J;

  EXPECT_DOUBLE_EQ(DenseJ(1, 0), 1.0);
  EXPECT_DOUBLE_EQ(DenseJ(1, 1), 0.0);
  EXPECT_DOUBLE_EQ(DenseJ(1, 2), -1.0);
  EXPECT_DOUBLE_EQ(DenseJ(1, 3), 0.0);

  EXPECT_DOUBLE_EQ(DenseJ(2, 0), 0.0);
  EXPECT_DOUBLE_EQ(DenseJ(2, 1), 1.0);
  EXPECT_DOUBLE_EQ(DenseJ(2, 2), 0.0);
  EXPECT_DOUBLE_EQ(DenseJ(2, 3), -1.0);
}

TEST_F(GasTEST, Source_evaluate) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow1start = -23.0;
  double flow1end = -440.0;

  double flow2start = -383.0;
  double flow2end = 81.0;


  auto node0_json = source_json("node0", flow0start, flow0end);
  auto node1_json = source_json("node1", flow1start, flow1end);
  auto node2_json = source_json("node2", flow2start, flow2end);

  auto shortpipe01_json =
    shortpipe_json("shortpipe01", node0_json, node1_json);
  auto shortpipe20_json =
    shortpipe_json("shortpipe20", node2_json, node0_json);

  auto np_json =
    make_full_json({node0_json, node1_json, node2_json}, {}, {},
                   {shortpipe01_json, shortpipe20_json}, {});

  auto netprob = get_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  double sp01_pressure_start = 810;
  double sp01_flow_start = -4;
  double sp01_pressure_end = 125;
  double sp01_flow_end = 1000;

  nlohmann::json initial01;
  {
    Eigen::Vector2d cond0(sp01_pressure_start,sp01_flow_start);
    Eigen::Vector2d cond1(sp01_pressure_end, sp01_flow_end);
    initial01 = make_value_json("shortpipe01","x",cond0,cond1);
  }
  double sp20_pressure_start = 811;
  double sp20_flow_start = -8;
  double sp20_pressure_end = 131;
  double sp20_flow_end = 1111;

  nlohmann::json initial20;
  {
    Eigen::Vector2d cond0(sp20_pressure_start, sp20_flow_start);
    Eigen::Vector2d cond1(sp20_pressure_end, sp20_flow_end);
    initial20 = make_value_json("shortpipe20", "x", cond0, cond1);
  }

  auto initial_json = make_initial_json({initial01,initial20},{});


  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);

  netprob->set_initial_values(new_state, initial_json);

  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);


    // node0:
  EXPECT_DOUBLE_EQ(rootvalues[0],-sp01_pressure_start + sp20_pressure_end);
  EXPECT_DOUBLE_EQ(rootvalues[7],-flow0start+sp01_flow_start- sp20_flow_end);

  //node 1:
  EXPECT_DOUBLE_EQ(rootvalues[3], -flow1start -sp01_flow_end);

  // node 2:
  EXPECT_DOUBLE_EQ(rootvalues[4], -flow2start + sp20_flow_start);

}


TEST_F(GasTEST, Source_evaluate_state_derivative) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow1start = -23.0;
  double flow1end = -440.0;

  double flow2start = -383.0;
  double flow2end = 81.0;


  auto node0_json = source_json("node0", flow0start, flow0end);
  auto node1_json = source_json("node1", flow1start, flow1end);
  auto node2_json = source_json("node2", flow2start, flow2end);

  auto shortpipe01_json =
    shortpipe_json("shortpipe01", node0_json, node1_json);
  auto shortpipe20_json =
    shortpipe_json("shortpipe20", node2_json, node0_json);

  auto np_json =
    make_full_json({node0_json, node1_json, node2_json}, {}, {},
                   {shortpipe01_json, shortpipe20_json}, {});

  auto netprob = get_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  double sp01_pressure_start = 810;
  double sp01_flow_start = -4;
  double sp01_pressure_end = 125;
  double sp01_flow_end = 1000;

  nlohmann::json initial01;
  {
    Eigen::Vector2d cond0(sp01_pressure_start,sp01_flow_start);
    Eigen::Vector2d cond1(sp01_pressure_end, sp01_flow_end);
    initial01 = make_value_json("shortpipe01","x",cond0,cond1);
  }
  double sp20_pressure_start = 811;
  double sp20_flow_start = -8;
  double sp20_pressure_end = 131;
  double sp20_flow_end = 1111;

  nlohmann::json initial20;
  {
    Eigen::Vector2d cond0(sp20_pressure_start, sp20_flow_start);
    Eigen::Vector2d cond1(sp20_pressure_end, sp20_flow_end);
    initial20 = make_value_json("shortpipe20", "x", cond0, cond1);
  }
  auto initial_json = make_initial_json({initial01,initial20},{});


  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);

  netprob->set_initial_values(new_state, initial_json);

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  netprob->evaluate_state_derivative(&handler, last_time, new_time, last_state,
                                     new_state);
  handler.set_matrix();

  Eigen::MatrixXd DenseJ = J;

  // node0:
  EXPECT_DOUBLE_EQ(DenseJ(0,0),-1);
  EXPECT_DOUBLE_EQ(DenseJ(0, 6), 1);
  for(Eigen::Index i= 0;i!=8;++i){
    if(i == 0 or i==6){
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(0,i),0.0);
  }

  EXPECT_DOUBLE_EQ(DenseJ(7, 1), 1);
  EXPECT_DOUBLE_EQ(DenseJ(7, 7), -1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 1 or i == 7) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(7, i), 0.0);
  }

  //node1:
  EXPECT_DOUBLE_EQ(DenseJ(3,3), -1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 3) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(3, i), 0.0);
  }

  //node2:
  EXPECT_DOUBLE_EQ(DenseJ(4, 5), 1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 5) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(4, i), 0.0);
  }
}



// TEST(testFlowboundarynode_Shortpipe, Flowboundarynode_three_shortpipes) {
//   SUCCEED();
// double flow0start = 88.0;
//   double flow0end = 10.0;

//   double flow3start = -723.0;
//   double flow3end = -640.0;

//   double flow2start = -23.0;
//   double flow2end = -440.0;

//   json flow_topology={};

//   json bd_json0 = {
//                    {"id", "gasnode0"},
//                    {"type", "flow"},
//                    {"data", json::array({{{"time", 0.},
//                                           {"values",
// json::array({flow0start})}},{{"time",
//                                           100.},{"values",
// json::array({flow0end})}}})}};

//   //std::cout << bd_json0 <<std::endl;

//   json bd_json2 = {
//                    {"id", "gasnode2"},
//                    {"type", "flow"},
//                    {"data", json::array({{{"time", 0.},
//                                           {"values",
// json::array({flow2start})}},{{"time",
//                                           100.},{"values",
// json::array({flow2end})}}})}};
//   //std::cout << bd_json1<<std::endl;

//   json bd_json3 = {
//                    {"id", "gasnode3"},
//                    {"type", "flow"},
//                    {"data", json::array({{{"time", 0.},
//                                           {"values",
// json::array({flow3start})}},{{"time",
//                                           100.},{"values",
// json::array({flow3end})}}})}};
//   //std::cout << bd_json1<<std::endl;

//   double sp0_pressure_start = 810;
//   double sp0_flow_start = -4;
//   double sp0_pressure_end = 125;
//   double sp0_flow_end = 1000;
//   json sp0_initial = {
//       {"id", "node_4_ld1"},
//       {"data",
//        json::array(
//            {{{"x", 0.0},
//              {"value", json::array({sp0_pressure_start,
//              sp0_flow_start})}},
//             {{"x", 1.0},
//              {"value", json::array({sp0_pressure_end,
//              sp0_flow_end})}}})}};
//   //std::cout << sp0_initial<<std::endl;

//   double sp1_pressure_start = 811;
//   double sp1_flow_start = -8;
//   double sp1_pressure_end = 131;
//   double sp1_flow_end = 1111;

// json sp1_initial = {
//     {"id", "node_4_ld1"},
//     {"data",
//      json::array(
//          {{{"x", 0.0},
//            {"value", json::array({sp1_pressure_start,
//            sp1_flow_start})}},
//           {{"x", 1.0},
//            {"value", json::array({sp1_pressure_end,
//            sp1_flow_end})}}})}};
// //std::cout << sp1_initial<<std::endl;

// double sp2_pressure_start = 822;
// double sp2_flow_start = -20;
// double sp2_pressure_end = 232;
// double sp2_flow_end = 2222;

//   json sp2_initial = {
//                       {"id", "node_4_ld2"},
//                       {"data",
//                        json::array(
//                                    {{{"x", 0.0},
//                                      {"value",
// json::array({sp2_pressure_start,
//                                      sp2_flow_start})}},
//                                     {{"x", 1.0},
//                                      {"value",
//                                      json::array({sp2_pressure_end,
//                                      sp2_flow_end})}}})}};
//   //std::cout << sp2_initial<<std::endl;

//   Model::Networkproblem::Gas::Flowboundarynode g0("gasnode0",
// bd_json0,
//   flow_topology); Model::Networkproblem::Gas::Innode g1("innode1");
//   Model::Networkproblem::Gas::Flowboundarynode g2("gasnode2",
// bd_json2,
//   flow_topology); Model::Networkproblem::Gas::Pressureboundarynode
//   g3("gasnode3", bd_json3, flow_topology);
//   Model::Networkproblem::Gas::Shortpipe sp0("SP0", &g0, &g1);
//   Model::Networkproblem::Gas::Shortpipe sp1("SP1", &g1, &g2);
//   Model::Networkproblem::Gas::Shortpipe sp2("SP1", &g1, &g3);

//   auto a = g0.set_indices(0);
//   auto b = g1.set_indices(a);
//   auto c = g2.set_indices(b);
//   auto c2 = g3.set_indices(c);
//   auto d = sp0.set_indices(c2);
//   auto f = sp1.set_indices(d);
//   auto e = sp2.set_indices(f);

// g0.setup();
// g1.setup();
// g2.setup();
// g3.setup();

//   double last_time = 0.0;
//   double new_time = 0.0;
//   Eigen::VectorXd rootvalues(e);
//   rootvalues.setZero();
//   Eigen::VectorXd last_state(e);
//   Eigen::VectorXd new_state(e);

//   sp0.set_initial_values(new_state, sp0_initial);
//   // std::cout << "Initial conditions sp0:" << std::endl;
//   // std::cout << new_state <<std::endl;

//   sp1.set_initial_values(new_state, sp1_initial);
//   // std::cout << "Initial conditions sp1:" << std::endl;
//   // std::cout << new_state <<std::endl;

//   sp2.set_initial_values(new_state, sp2_initial);
//   // std::cout << "Initial conditions sp2:" << std::endl;
//   // std::cout << new_state <<std::endl;

//   g0.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);
//   //   // std::cout << "Evaluation  g0:" << std::endl;
//   //   // std::cout << rootvalues << std::endl;

//   //   g1.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);
//   //   // std::cout << "Evaluation  g1:" << std::endl;
//   //   // std::cout << rootvalues << std::endl;

//   //   g2.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);
//   //   // std::cout << "Evaluation  g2:" << std::endl;
//   //   // std::cout << rootvalues << std::endl;

//   //   g3.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);
//   //   // std::cout << "Evaluation  g3:" << std::endl;
//   //   // std::cout << rootvalues << std::endl;

//   //   sp0.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);
//   //   // std::cout << "Evaluation shortpipe0:" << std::endl;
//   //   // std::cout << rootvalues << std::endl;

//   //   sp1.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);
//   //   // std::cout << "Evaluation shortpipe1:" << std::endl;
//   //   // std::cout << rootvalues << std::endl;

//   //   sp2.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);
//   //   // std::cout << "Evaluation shortpipe2:" << std::endl;
//   //   // std::cout << rootvalues << std::endl;

//   //   // inner node (multiple pipes connected)

//   //   Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
//   //   J.setZero();
//   //   Aux::Triplethandler handler(&J);

//   //   g0.evaluate_state_derivative(&handler, last_time, new_time,
//   last_state,
//   //                                new_state);
//   //   g1.evaluate_state_derivative(&handler, last_time, new_time,
//   last_state,
//   //                                new_state);
//   //   g2.evaluate_state_derivative(&handler, last_time, new_time,
//   last_state,
//   //                                new_state);
//   //   g3.evaluate_state_derivative(&handler, last_time, new_time,
//   last_state,
//   //                                new_state);
//   //   sp0.evaluate_state_derivative(&handler, last_time, new_time,
//   //   last_state,new_state); sp1.evaluate_state_derivative(&handler,
//   //   last_time, new_time, last_state,new_state);
//   //   sp2.evaluate_state_derivative(&handler, last_time, new_time,
//   //   last_state,new_state);

//   //   handler.set_matrix();

//   //   // std::cout << J << std::endl;

//   //   // shortpipes: 3 * 4
//   //   // outer nodes: 6
//   //   // inner node flow: 6
//   //   // inner node pressure: 8
//   //   EXPECT_EQ(J.nonZeros(),3*4+6+6+8);

//   //   Eigen::MatrixXd jd = J;
//   //   // std::cout << jd.inverse().sparseView()<< std::endl;
//   //   std::cout <<  "\033[1;31mMaybe also test evaluate in this
//   multi-edge
//   //   setting.\033[0m\n" <<std::endl;
// }

// TEST(testPipe, evaluate) {
//   SUCCEED();
//   //   double flow0start = 88.0;
//   //   double flow0end = 10.0;

//   //   double flow1start = -23.0;
//   //   double flow1end = -440.0;

//   //   json flow_topology={};

//   //   json bd_json0 = {
//   //                    {"id", "gasnode0"},
//   //                    {"type", "flow"},
//   //                    {"data", json::array({{{"time", 0.},
//   //                                           {"values",
//   // json::array({flow0start})}},{{"time",
//   //                                           100.},{"values",
//   // json::array({flow0end})}}})}};

//   //   //std::cout << bd_json0 <<std::endl;
//   //   json bd_json1 = {
//   //                    {"id", "gasnode1"},
//   //                    {"type", "flow"},
//   //                    {"data", json::array({{{"time", 0.},
//   //                                           {"values",
//   // json::array({flow1start})}},{{"time",
//   //                                           100.},{"values",
//   // json::array({flow1end})}}})}};
//   //   //std::cout << bd_json1<<std::endl;

//   //   double pressure_start = 810;
//   //   double pressure_end = 125;
//   //   double flow_start =-4;
//   //   double flow_end = 1000;
//   //   json p0_initial = {
//   //                       {"id", "node_4_ld1"},
//   //                       {"data", json::array({{{"x", 0.0},
//   //                                              {"value",
//   // json::array({pressure_start,flow_start})}},{{"x",
//   //                                              1.0},{"value",
//   // json::array({pressure_end,flow_end})}}})}};
//   //   //std::cout << sp0_initial<<std::endl;

//   //   std::string pipe_topology_string =
//   //     "{\n"
//   //     "\"id\": \"p_br2\",\n"
//   //     "\"from\": \"node_2\",\n"
//   //     "\"to\": \"node_3\",\n"
//   //     "\"heatTransferCoefficient\": \"2\",\n"
//   //     "\"flowMin\": {\n"
//   //     "\"unit\": \"m_cube_per_s\",\n"
//   //     "\"value\": \"-6369.426752\"\n"
//   //     "},\n"
//   //     "\"flowMax\": {\n"
//   //     "\"unit\": \"m_cube_per_s\",\n"
//   //     "\"value\": \"6369.426752\"\n"
//   //     "},\n"
//   //     "\"flowInInit\": {\n"
//   //     "\"unit\": \"m_cube_per_s\",\n"
//   //     "\"value\": \"-6369.426752\"\n"
//   //     "},\n"
//   //     "\"flowOutInit\": {\n"
//   //     "\"unit\": \"m_cube_per_s\",\n"
//   //     "\"value\": \"-6369.426752\"\n"
//   //     "},\n"
//   //     "\"length\": {\n"
//   //     "\"unit\": \"km\",\n"
//   //     "\"value\": \"15.25\"\n"
//   //     "},\n"
//   //     "\"diameter\": {\n"
//   //     "\"unit\": \"mm\",\n"
//   //     "\"value\": \"914.400000\"\n"
//   //     "},\n"
//   //     "\"roughness\": {\n"
//   //     "\"unit\": \"m\",\n"
//   //     "\"value\": \"0.000008\"\n"
//   //     "}\n"
//   //     "}";
//   //   json pipe_topology = json::parse(pipe_topology_string);

//   //   std::string pipe_initial_string ="{\n"
//   //     " \"id\": \"p_br2\",\n"
//   //     " \"data\": [\n"
//   //     "{\n"
//   //     "  \"x\": 0.0,\n"
//   //     "    \"value\": [\n"
//   //     "              75.046978,\n"
//   //     "              58.290215\n"
//   //     "              ]\n"
//   //     "    },\n"
//   //     "{\n"
//   //     "  \"x\": 3812.5,\n"
//   //     "    \"value\": [\n"
//   //     "              75.032557,\n"
//   //     "              58.105963\n"
//   //     "              ]\n"
//   //     "    },\n"
//   //     "{\n"
//   //     "  \"x\": 7625.0,\n"
//   //     "    \"value\": [\n"
//   //     "              75.01822,\n"
//   //     "              57.921692\n"
//   //     "              ]\n"
//   //     "    },\n"
//   //     "{\n"
//   //     "  \"x\": 11437.5,\n"
//   //     "    \"value\": [\n"
//   //     "              75.003966,\n"
//   //     "              57.737405\n"
//   //     "              ]\n"
//   //     "    },\n"
//   //     "{\n"
//   //     "  \"x\": 15250.0,\n"
//   //     "    \"value\": [\n"
//   //     "              74.989795,\n"
//   //     "              57.553105\n"
//   //     "              ]\n"
//   //     "    }\n"
//   //     "          ]\n"
//   //     "}";

//   //   json pipe_initial = json::parse(pipe_initial_string);
//   //   double Delta_x = 20000;

//   //   Model::Networkproblem::Gas::Flowboundarynode  g0("gasnode0",
//   bd_json0,
//   //   flow_topology); Model::Networkproblem::Gas::Flowboundarynode
//   //   g1("gasnode1", bd_json1, flow_topology);
//   //   Model::Networkproblem::Gas::Pipe p0("P0", &g0, &g1, pipe_topology,
//   //   Delta_x);

//   //   auto a = g0.set_indices(0);
//   //   auto b = g1.set_indices(a);
//   //   auto c = p0.set_indices(b);

//   //   g0.setup();
//   //   g1.setup();

//   //   double last_time = 0.0;
//   //   double new_time = 10.0;
//   //   Eigen::VectorXd rootvalues(c);
//   //   rootvalues.setZero();
//   //   Eigen::VectorXd last_state(c);
//   //   Eigen::VectorXd new_state(c);

//   //   p0.set_initial_values(new_state,pipe_initial);
//   //   last_state = new_state;
//   //   // std::cout << "Initial conditions:" << std::endl;
//   //   // std::cout << new_state <<std::endl;

//   //   // std::cout << p0.get_boundary_p_qvol_bar(1,new_state)
//   <<std::endl;
//   //   // std::cout << p0.get_boundary_p_qvol_bar(-1,new_state)
//   <<std::endl;

//   //   p0.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);

//   //   Eigen::Vector2d last_left = last_state.segment<2>(0);
//   //   Eigen::Vector2d last_right = last_state.segment<2>(2);

//   //   Eigen::Vector2d new_left = new_state.segment<2>(0);
//   //   Eigen::Vector2d new_right = new_state.segment<2>(2);

//   //   Eigen::Vector2d expected_result;

//   //   double actual_Delta_x = p0.Delta_x;

//   //   p0.scheme.evaluate_point(expected_result, last_time, new_time,
//   //   actual_Delta_x,
//   //                            last_left,
//   //                            last_right,
//   //                            new_left,
//   //                            new_right,
//   //                            p0.bl);

//   //   EXPECT_DOUBLE_EQ(expected_result[0],rootvalues.segment<2>(1)[0]);
//   //   EXPECT_DOUBLE_EQ(expected_result[1],rootvalues.segment<2>(1)[1]);

//   // }

//   // TEST(testPipe, evaluate_state_derivative) {
//   //   double flow0start = 88.0;
//   //   double flow0end = 10.0;

//   //   double flow1start = -23.0;
//   //   double flow1end = -440.0;

//   //   json flow_topology={};

//   //   json bd_json0 = {
//   //                    {"id", "gasnode0"},
//   //                    {"type", "flow"},
//   //                    {"data", json::array({{{"time", 0.},
//   //                                           {"values",
//   // json::array({flow0start})}},{{"time",
//   //                                           100.},{"values",
//   // json::array({flow0end})}}})}};

//   //   //std::cout << bd_json0 <<std::endl;
//   //   json bd_json1 = {
//   //                    {"id", "gasnode1"},
//   //                    {"type", "flow"},
//   //                    {"data", json::array({{{"time", 0.},
//   //                                           {"values",
//   // json::array({flow1start})}},{{"time",
//   //                                           100.},{"values",
//   // json::array({flow1end})}}})}};
//   //   //std::cout << bd_json1<<std::endl;

//   //   double pressure_start = 810;
//   //   double pressure_end = 125;
//   //   double flow_start =-4;
//   //   double flow_end = 1000;
//   //   json p0_initial = {
//   //                       {"id", "node_4_ld1"},
//   //                       {"data", json::array({{{"x", 0.0},
//   //                                              {"value",
//   // json::array({pressure_start,flow_start})}},{{"x",
//   //                                              1.0},{"value",
//   // json::array({pressure_end,flow_end})}}})}};
//   //   //std::cout << sp0_initial<<std::endl;

//   //   std::string pipe_topology_string =
//   //     "{\n"
//   //     "\"id\": \"p_br2\",\n"
//   //     "\"from\": \"node_2\",\n"
//   //     "\"to\": \"node_3\",\n"
//   //     "\"heatTransferCoefficient\": \"2\",\n"
//   //     "\"flowMin\": {\n"
//   //     "\"unit\": \"m_cube_per_s\",\n"
//   //     "\"value\": \"-6369.426752\"\n"
//   //     "},\n"
//   //     "\"flowMax\": {\n"
//   //     "\"unit\": \"m_cube_per_s\",\n"
//   //     "\"value\": \"6369.426752\"\n"
//   //     "},\n"
//   //     "\"flowInInit\": {\n"
//   //     "\"unit\": \"m_cube_per_s\",\n"
//   //     "\"value\": \"-6369.426752\"\n"
//   //     "},\n"
//   //     "\"flowOutInit\": {\n"
//   //     "\"unit\": \"m_cube_per_s\",\n"
//   //     "\"value\": \"-6369.426752\"\n"
//   //     "},\n"
//   //     "\"length\": {\n"
//   //     "\"unit\": \"km\",\n"
//   //     "\"value\": \"15.25\"\n"
//   //     "},\n"
//   //     "\"diameter\": {\n"
//   //     "\"unit\": \"mm\",\n"
//   //     "\"value\": \"914.400000\"\n"
//   //     "},\n"
//   //     "\"roughness\": {\n"
//   //     "\"unit\": \"m\",\n"
//   //     "\"value\": \"0.000008\"\n"
//   //     "}\n"
//   //     "}";
//   //   json pipe_topology = json::parse(pipe_topology_string);

//   //   std::string pipe_initial_string ="{\n"
//   //     " \"id\": \"p_br2\",\n"
//   //     " \"data\": [\n"
//   //     "{\n"
//   //     "  \"x\": 0.0,\n"
//   //     "    \"value\": [\n"
//   //     "              75.046978,\n"
//   //     "              58.290215\n"
//   //     "              ]\n"
//   //     "    },\n"
//   //     "{\n"
//   //     "  \"x\": 3812.5,\n"
//   //     "    \"value\": [\n"
//   //     "              75.032557,\n"
//   //     "              58.105963\n"
//   //     "              ]\n"
//   //     "    },\n"
//   //     "{\n"
//   //     "  \"x\": 7625.0,\n"
//   //     "    \"value\": [\n"
//   //     "              75.01822,\n"
//   //     "              57.921692\n"
//   //     "              ]\n"
//   //     "    },\n"
//   //     "{\n"
//   //     "  \"x\": 11437.5,\n"
//   //     "    \"value\": [\n"
//   //     "              75.003966,\n"
//   //     "              57.737405\n"
//   //     "              ]\n"
//   //     "    },\n"
//   //     "{\n"
//   //     "  \"x\": 15250.0,\n"
//   //     "    \"value\": [\n"
//   //     "              74.989795,\n"
//   //     "              57.553105\n"
//   //     "              ]\n"
//   //     "    }\n"
//   //     "          ]\n"
//   //     "}";

//   //   json pipe_initial = json::parse(pipe_initial_string);
//   //   double Delta_x = 20000;

//   //   Model::Networkproblem::Gas::Flowboundarynode  g0("gasnode0",
//   bd_json0,
//   //   flow_topology); Model::Networkproblem::Gas::Flowboundarynode
//   //   g1("gasnode1", bd_json1, flow_topology);
//   //   Model::Networkproblem::Gas::Pipe p0("P0", &g0, &g1, pipe_topology,
//   //   Delta_x);

//   //   auto a = g0.set_indices(0);
//   //   auto b = g1.set_indices(a);
//   //   auto c = p0.set_indices(b);

//   // g0.setup();
//   // g1.setup();

//   //   double last_time = 0.0;
//   //   double new_time = 10.0;
//   //   Eigen::VectorXd rootvalues(c);
//   //   Eigen::VectorXd rootvalues0(c);
//   //   Eigen::VectorXd rootvaluesm0(c);
//   //   Eigen::VectorXd rootvalues1(c);
//   //   Eigen::VectorXd rootvalues2(c);
//   //   Eigen::VectorXd rootvalues3(c);

//   //   double epsilon = pow(DBL_EPSILON,1.0/3.0);

//   //   // I'm reasonably sure that is is ok

//   //   // std::cout <<RED<< __FILE__ <<":"<<__LINE__ <<": This is a hack!
//   //   Maybe its ok, to use twice the squareroot but check this!" <<
//   //   RESET<<std::endl;

//   //   std::cout << RED << __FILE__ << ":" << __LINE__
//   //             << ": Be aware that here twice the squareroot of machine
//   //             epsilon is used for finite difference comparison!"
//   //             << RESET << std::endl;
//   //   double finite_difference_threshold = 2 * sqrt(epsilon);

//   //   Eigen::VectorXd h0(c);
//   //   h0 << epsilon,0,0,0;
//   //   Eigen::VectorXd h1(c);
//   //   h1 << 0,epsilon,0,0;
//   //   Eigen::VectorXd h2(c);
//   //   h2 << 0,0,epsilon,0;
//   //   Eigen::VectorXd h3(c);
//   //   h3 << 0,0,0,epsilon;

//   //   rootvalues.setZero();
//   //   rootvalues0.setZero();
//   //   rootvaluesm0.setZero();
//   //   rootvalues1.setZero();
//   //   rootvalues2.setZero();
//   //   rootvalues3.setZero();
//   //   Eigen::VectorXd last_state(c);
//   //   Eigen::VectorXd new_state(c);

//   //   p0.set_initial_values(new_state,pipe_initial);
//   //   last_state = new_state;

//   //   p0.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);
//   //   p0.evaluate(rootvalues0, last_time, new_time, last_state,
//   //   new_state+h0); p0.evaluate(rootvaluesm0, last_time, new_time,
//   //   last_state, new_state-h0); p0.evaluate(rootvalues1, last_time,
//   //   new_time, last_state, new_state+h1); p0.evaluate(rootvalues2,
//   //   last_time, new_time, last_state, new_state+h2);
//   //   p0.evaluate(rootvalues3, last_time, new_time, last_state,
//   //   new_state+h3);

//   //   Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
//   //   J.setZero();
//   //   Aux::Triplethandler handler(&J);

//   //   p0.evaluate_state_derivative(&handler, last_time, new_time,
//   last_state,
//   //                                new_state);
//   //   handler.set_matrix();

//   //   Eigen::VectorXd diff0 =
//   //   (0.5*(rootvalues0-rootvaluesm0)/epsilon).segment<2>(1);;
//   //   Eigen::VectorXd diff1 =
//   //   ((rootvalues1-rootvalues)/epsilon).segment<2>(1); Eigen::VectorXd
//   diff2
//   //   = ((rootvalues2-rootvalues)/epsilon).segment<2>(1);
//   Eigen::VectorXd
//   //   diff3 = ((rootvalues3-rootvalues)/epsilon).segment<2>(1);

//   //   Eigen::SparseMatrix<double> J_relevant(2,4);
//   //   J_relevant= J.block(1,0,2,4);

//   //   Eigen::Matrix<double,2,4> expected_J;
//   //       Eigen::Matrix2d id;
//   //       id.setIdentity();

//   //       double Delta_t = new_time-last_time;
//   //       double actual_Delta_x = p0.Delta_x;

//   //       Model::Balancelaw::Isothermaleulerequation bl(p0.bl);

//   //       Eigen::Vector2d new_left = new_state.segment<2>(0);
//   //       Eigen::Vector2d new_right = new_state.segment<2>(2);

//   //       expected_J.block<2,2>(0,0) = 0.5*id-
//   //       Delta_t/actual_Delta_x*bl.dflux_dstate(new_left)
//   //       -0.5*Delta_t*bl.dsource_dstate(new_left); expected_J.block<2,
//   2>(0,
//   //       2) =
//   //           0.5 * id + Delta_t / actual_Delta_x *
//   //           bl.dflux_dstate(new_right) - 0.5 * Delta_t *
//   //           bl.dsource_dstate(new_right);

//   //       // Eigen::Matrix<double,2,4> Jdense = J_relevant;

//   //       //
//   // EXPECT_DOUBLE_EQ((Jdense-expected_J).lpNorm<Eigen::Infinity>(),0);

//   //       Eigen::VectorXd analytical0 = J_relevant * h0 / epsilon;
//   //       Eigen::VectorXd analytical1 = J_relevant * h1 / epsilon;
//   //       Eigen::VectorXd analytical2 = J_relevant * h2 / epsilon;
//   //       Eigen::VectorXd analytical3 = J_relevant * h3 / epsilon;

//   //       double max0 = (diff0 - analytical0).lpNorm<Eigen::Infinity>();
//   //       double max1 = (diff1 - analytical1).lpNorm<Eigen::Infinity>();
//   //       double max2 = (diff2 - analytical2).lpNorm<Eigen::Infinity>();
//   //       double max3 = (diff3 - analytical3).lpNorm<Eigen::Infinity>();

//   //       std::vector<double> maxima = {max0, max1, max2, max3};
//   //       double max = *(std::max_element(maxima.begin(),
//   maxima.end()));

//   //       EXPECT_NEAR(0, max, finite_difference_threshold);
// }

// TEST(testGaspowerconnection, evaluate) {
//   SUCCEED();
//   //   double G1 = 1.0;
//   //   double B1 = -5.0;

//   //   double power2gas_q_coefficient = 0.4356729;
//   //   double gas2power_q_coefficient = 0.1256;
//   //   json gp_topology = {
//   //       {"from", "gasnode0"},
//   //       {"id", "gp0"},
//   //       {"to", "N1"},
//   //       {"power2gas_q_coeff",
//   //       Aux::to_string_precise(power2gas_q_coefficient)},
//   //       {"gas2power_q_coeff",
//   //       Aux::to_string_precise(gas2power_q_coefficient)}};

//   //   double flow0start = 88.0;
//   //   double flow0end = 10.0;

//   //   double pressure_init = 50;
//   //   double flow_init = -1.0/gas2power_q_coefficient;

//   //   json flow_topology={};

//   //   json bd_gas0 = {
//   //                    {"id", "gasnode0"},
//   //                    {"type", "flow"},
//   //                    {"data", json::array({{{"time", 0.},
//   //                                           {"values",
//   // json::array({flow0start})}},{{"time",
//   //                                           100.},{"values",
//   // json::array({flow0end})}}})}};

//   //   json gp0_initial =
//   //     {
//   //      {"id", "node_4_ld1"},
//   //      {"data", {
//   //                {"x", 0.0},
//   //                {"value", json::array(
//   //                                      {pressure_init,flow_init}
//   //                                      )
//   //                }
//   //        }
//   //      }
//   //     };

//   //   double V1_bd = 8.0;
//   //   double phi1_bd = 6.0;

//   //   double V1 = 18.0;
//   //   double phi1 = 10;

//   //   json power_initial = {
//   //                         {"id", "N1"},
//   //                         {"data", {
//   //                                   {"x", 0.000000},
//   //                                   {"value", json::array( {-0.810000,
//   //                                                           -0.441000,
//   //                                                           V1,
//   //                                                           phi1
//   //                                       })}
//   //                           }}
//   //   };

//   //   json bd_power1 = {
//   //                    {"id", "N1"},
//   //                    {"type", "Vphi"},
//   //                    {"data", json::array({{{"time", 0.},
//   //                                           {"values",
//   json::array({V1_bd,
//   //                                           phi1_bd})}}})}};

//   //   Model::Networkproblem::Gas::Flowboundarynode  g0("gasnode0",
//   bd_gas0,
//   //   flow_topology); Model::Networkproblem::Power::Vphinode  N1("N1",
//   //   bd_power1, G1,B1); Model::Networkproblem::Gas::Gaspowerconnection
//   //   gp0("gp0", &g0,&N1, gp_topology);

//   //   int a = g0.set_indices(0);
//   //   int b = N1.set_indices(a);
//   //   int c = gp0.set_indices(b);

//   //   g0.setup();
//   //   N1.setup();
//   //   gp0.setup();
//   // Eigen::VectorXd rootvalues(c);
//   // Eigen::VectorXd last_state(c);
//   // Eigen::VectorXd new_state(c);

//   // N1.set_initial_values(new_state, power_initial);
//   // gp0.set_initial_values(new_state, gp0_initial);

//   // EXPECT_DOUBLE_EQ(new_state[0], V1);
//   // EXPECT_DOUBLE_EQ(new_state[1], phi1);
//   // EXPECT_DOUBLE_EQ(new_state[2], pressure_init);
//   // EXPECT_DOUBLE_EQ(new_state[3], flow_init);

//   // double last_time = 0.0;
//   // double new_time = 1.0;

//   // for (int i = -10; i != 20; ++i) {

//   //   double V = i;
//   //   double phi = 3 * i;
//   //   double p = i * i;
//   //   double q = 0.5 * i;
//   //   new_state << V, phi, p, q;
//   //   // std::cout << new_state << std::endl;
//   //   rootvalues.setZero();

//   //   gp0.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state);

//   //   EXPECT_DOUBLE_EQ(rootvalues[2], 0.0);

//   //   EXPECT_DOUBLE_EQ(rootvalues[3], (-gp0.generated_power(q) +
//   //   N1.P(new_state)));

//   //   }
// }

// TEST(testGaspowerconnection, evaluate_state_derivative) {
//   SUCCEED();
//   //   double G1 = 1.0;
//   //   double B1 = -5.0;

//   //   double power2gas_q_coefficient = 0.4356729;
//   //   double gas2power_q_coefficient = 0.1256;
//   //   json gp_topology = {
//   //       {"from", "gasnode0"},
//   //       {"id", "gp0"},
//   //       {"to", "N1"},
//   //       {"power2gas_q_coeff",
//   //       Aux::to_string_precise(power2gas_q_coefficient)},
//   //       {"gas2power_q_coeff",
//   //       Aux::to_string_precise(gas2power_q_coefficient)}};

//   //   double flow0start = 88.0;
//   //   double flow0end = 10.0;

//   //   double pressure_init = 50;
//   //   double flow_init = -1.0/gas2power_q_coefficient;

//   //   json flow_topology={};

//   //   json bd_gas0 = {
//   //                    {"id", "gasnode0"},
//   //                    {"type", "flow"},
//   //                    {"data", json::array({{{"time", 0.},
//   //                                           {"values",
//   // json::array({flow0start})}},{{"time",
//   //                                           100.},{"values",
//   // json::array({flow0end})}}})}};

//   //   json gp0_initial =
//   //     {
//   //      {"id", "node_4_ld1"},
//   //      {"data", {
//   //                {"x", 0.0},
//   //                {"value", json::array(
//   //                                      {pressure_init,flow_init}
//   //                                      )
//   //                }
//   //        }
//   //      }
//   //     };

//   //   double V1_bd = 8.0;
//   //   double phi1_bd = 6.0;

//   //   double V1 = 18.0;
//   //   double phi1 = 10;

//   //   json power_initial = {
//   //                         {"id", "N1"},
//   //                         {"data", {
//   //                                   {"x", 0.000000},
//   //                                   {"value", json::array( {-0.810000,
//   //                                                           -0.441000,
//   //                                                           V1,
//   //                                                           phi1
//   //                                       })}
//   //                           }}
//   //   };

//   //   json bd_power1 = {
//   //                    {"id", "N1"},
//   //                    {"type", "Vphi"},
//   //                    {"data", json::array({{{"time", 0.},
//   //                                           {"values",
//   json::array({V1_bd,
//   //                                           phi1_bd})}}})}};

//   //   Model::Networkproblem::Gas::Flowboundarynode  g0("gasnode0",
//   bd_gas0,
//   //   flow_topology); Model::Networkproblem::Power::Vphinode  N1("N1",
//   //   bd_power1, G1,B1); Model::Networkproblem::Gas::Gaspowerconnection
//   //   gp0("gp0", &g0,&N1, gp_topology);

//   //   int a = g0.set_indices(0);
//   //   int b = N1.set_indices(a);
//   //   int c = gp0.set_indices(b);

//   // g0.setup();
//   // N1.setup();
//   // gp0.setup();

//   //  Eigen::VectorXd rootvalues(c);
//   // Eigen::VectorXd rootvaluesh(c);
//   // Eigen::VectorXd last_state(c);
//   // Eigen::VectorXd new_state(c);

//   // N1.set_initial_values(new_state, power_initial);
//   // gp0.set_initial_values(new_state, gp0_initial);

//   // EXPECT_DOUBLE_EQ(new_state[0], V1);
//   // EXPECT_DOUBLE_EQ(new_state[1], phi1);
//   // EXPECT_DOUBLE_EQ(new_state[2], pressure_init);
//   // EXPECT_DOUBLE_EQ(new_state[3], flow_init);

//   // double last_time = 0.0;
//   // double new_time = 0.0;

//   // double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
//   // Eigen::Vector4d h;
//   // h << 0, 0, 0, epsilon;
//   // double finite_difference_threshold = sqrt(epsilon);
//   // for (int i = -10; i != 20; ++i) {

//   //   new_state << i, 3 * i, i * i, 0.5 * i;
//   //   // std::cout << new_state << std::endl;
//   //   rootvalues.setZero();

//   //   gp0.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state -
//   //   h); N1.evaluate(rootvalues, last_time, new_time, last_state,
//   new_state
//   //   - h); g0.evaluate(rootvalues, last_time, new_time, last_state,
//   //   new_state - h);

//   //   gp0.evaluate(rootvaluesh, last_time, new_time, last_state,
//   new_state +
//   //   h); N1.evaluate(rootvaluesh, last_time, new_time, last_state,
//   new_state
//   //   + h); g0.evaluate(rootvaluesh, last_time, new_time, last_state,
//   //   new_state + h);

//   //   Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
//   //   Aux::Triplethandler handler(&J);

//   //   gp0.evaluate_state_derivative(&handler, last_time, new_time,
//   //   last_state,
//   //                                 new_state);
//   //   N1.evaluate_state_derivative(&handler, last_time, new_time,
//   last_state,
//   //                                new_state);
//   //   g0.evaluate_state_derivative(&handler, last_time, new_time,
//   last_state,
//   //                                new_state);

//   //   handler.set_matrix();

//   //   Eigen::Vector4d der = 0.5 * (rootvaluesh - rootvalues) / epsilon;
//   //   Eigen::Vector4d analytical_der = J * h / epsilon;

//   //   double max = (der - analytical_der).lpNorm<Eigen::Infinity>();
//   //   EXPECT_NEAR(max, 0, finite_difference_threshold);

//   //   }
// }

// TEST(testGaspowerconnection, generated_power) {
//   SUCCEED();
//   // double G1 = 1.0;
//   // double B1 = -5.0;
//   // double V1_bd = 8.0;
//   // double phi1_bd = 6.0;

//   // double power2gas_q_coefficient = 0.4356729;
//   // double gas2power_q_coefficient = 0.1256;
//   // json gp_topology = {
//   //     {"from", "gasnode0"},
//   //     {"id", "gp0"},
//   //     {"to", "N1"},
//   //     {"power2gas_q_coeff",
//   //     Aux::to_string_precise(power2gas_q_coefficient)},
//   //     {"gas2power_q_coeff",
//   //     Aux::to_string_precise(gas2power_q_coefficient)}};

//   // double flow0start = 88.0;
//   // double flow0end = 10.0;

//   // json flow_topology={};

//   // json bd_gas0 = {
//   //                  {"id", "gasnode0"},
//   //                  {"type", "flow"},
//   //                  {"data", json::array({{{"time", 0.},
//   //                                         {"values",
//   // json::array({flow0start})}},{{"time",
//   //                                         100.},{"values",
//   // json::array({flow0end})}}})}};

//   // json bd_power1 = {
//   //                  {"id", "N1"},
//   //                  {"type", "Vphi"},
//   //                  {"data", json::array({{{"time", 0.},
//   //                                         {"values",
//   json::array({V1_bd,
//   //                                         phi1_bd})}}})}};

//   // Model::Networkproblem::Gas::Flowboundarynode  g0("gasnode0",
//   bd_gas0,
//   // flow_topology); Model::Networkproblem::Power::Vphinode  N1("N1",
//   // bd_power1, G1,B1); Model::Networkproblem::Gas::Gaspowerconnection
//   // gp0("gp0", &g0,&N1, gp_topology);

//   // EXPECT_DOUBLE_EQ(gp0.smoothing_polynomial(gp0.kappa),
//   // gas2power_q_coefficient*gp0.kappa );
//   // EXPECT_DOUBLE_EQ(gp0.smoothing_polynomial(-gp0.kappa),
//   // power2gas_q_coefficient*(-gp0.kappa) );
//   // EXPECT_DOUBLE_EQ(gp0.dsmoothing_polynomial_dq(gp0.kappa),
//   // gas2power_q_coefficient  );
//   // EXPECT_DOUBLE_EQ(gp0.dsmoothing_polynomial_dq(-gp0.kappa),
//   // power2gas_q_coefficient );

//   // // for (int i=-10;i!=11;++i){
//   // //   std::cout << gp0.generated_power(i) <<std::endl;

//   // //   std::cout << gp0.dgenerated_power_dq(i) <<std::endl;
//   // // }
// }



template <typename Derived>
void Eigen::to_json(nlohmann::json &j, const MatrixBase<Derived> &eig) {
  EIGEN_STATIC_ASSERT_VECTOR_ONLY(MatrixBase<Derived>);
  // static_assert(eig.cols() == 1,
  //               "This function is only for vectors, not for
  //               matrices.");
  j = nlohmann::json::array();

  for (Eigen::Index i = 0; i != eig.rows(); ++i) {
    j.push_back(eig(i));
  }
}

template <typename Derived0, typename Derived1>
nlohmann::json make_value_json(std::string id, std::string key,
                               Eigen::MatrixBase<Derived0> const &condition0,
                               Eigen::MatrixBase<Derived1> const &condition1) {
  EIGEN_STATIC_ASSERT_VECTOR_ONLY(Eigen::MatrixBase<Derived0>);
  EIGEN_STATIC_ASSERT_VECTOR_ONLY(Eigen::MatrixBase<Derived1>);
  nlohmann::json bound;
  bound["id"] = id;
  bound["data"] = nlohmann::json::array();
  nlohmann::json b0;
  b0[key] = 0;
  b0["values"] = condition0;
  nlohmann::json b1;
  b1[key] = 1;
  b1["values"] = condition1;
  bound["data"].push_back(b0);
  bound["data"].push_back(b1);

  return bound;
}

nlohmann::json make_value_json(std::string id, std::string key,
                               double condition0, double condition1) {
  Eigen::Matrix<double, 1, 1> cond0(condition0);
  Eigen::Matrix<double, 1, 1> cond1(condition1);
  return make_value_json(id, key, cond0, cond1);
}

nlohmann::json make_initial_json(std::vector<nlohmann::json> shortpipes,
                                 std::vector<nlohmann::json> pipes){
  nlohmann::json initial_json;

  if (not pipes.empty()) {
    for (auto &pipe : pipes) {
      initial_json["connections"]["Pipe"].push_back(pipe);
    }
  }
  if (not shortpipes.empty()) {
    for (auto &shortpipe : shortpipes) {
      initial_json["connections"]["Shortpipe"].push_back(
          shortpipe);
    }
  }

  return initial_json;
}

nlohmann::json make_full_json(std::vector<nlohmann::json> sources,
                              std::vector<nlohmann::json> sinks,
                              std::vector<nlohmann::json> innodes,
                              std::vector<nlohmann::json> shortpipes,
                              std::vector<nlohmann::json> pipes) {
  nlohmann::json full_json;
  full_json["boundary_json"] = nlohmann::json::object_t();
  full_json["control_json"] = nlohmann::json::object_t();
  full_json["topology_json"] = nlohmann::json::object_t();
  if (not sources.empty()) {
    for (auto &source : sources) {
      full_json["topology_json"]["nodes"]["Source"].push_back(source);
    }
  }
if (not sinks.empty()) {
  for (auto &sink : sinks) {
    full_json["topology_json"]["nodes"]["Sink"].push_back(sink);
  }
}
if (not innodes.empty()) {
  for (auto &innode : innodes) {
    full_json["topology_json"]["nodes"]["Innode"].push_back(innode);
  }
}

if (not pipes.empty()) {
  for (auto &pipe : pipes) {
    full_json["topology_json"]["connections"]["Pipe"].push_back(pipe);
  }
}
if (not shortpipes.empty()) {
  for (auto &shortpipe : shortpipes) {
    full_json["topology_json"]["connections"]["Shortpipe"].push_back(shortpipe);
  }
}

 return full_json;
}

nlohmann::json source_json(std::string id, double flowstart, double flowend) {
  nlohmann::json topology;
  topology["id"] = id;
  auto b0 = make_value_json(topology["id"], "time", flowstart, flowend);
  topology["boundary_values"] = b0;
  return topology;
}
nlohmann::json sink_json(std::string id, double flowstart, double flowend) {
  return source_json(id, flowstart, flowend);
}
nlohmann::json shortpipe_json(std::string id,nlohmann::json startnode,
                              nlohmann::json endnode) {

  nlohmann::json shortpipe_topology;
  shortpipe_topology["id"] = id;
  shortpipe_topology["from"] = startnode["id"];
  shortpipe_topology["to"] = endnode["id"];

  return shortpipe_topology;
}
