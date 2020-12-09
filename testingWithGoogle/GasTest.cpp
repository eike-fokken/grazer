#include <Pipe.hpp>
#include <Flowboundarynode.hpp>
#include <Pressureboundarynode.hpp>
#include <Innode.hpp>
#include <Shortpipe.hpp>
#include "Matrixhandler.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::ordered_json;


TEST(testFlowboundarynode_Shortpipe, evaluate_and_evaluate_state_derivative) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow1start = -23.0;
  double flow1end = -440.0;

  json flow_topology={};

  json bd_json0 = {
                   {"id", "gasnode0"},
                   {"type", "flow"},
                   {"data", json::array({{{"time", 0.},
                                          {"values", json::array({flow0start})}},{{"time", 100.},{"values", json::array({flow0end})}}})}};

  //std::cout << bd_json0 <<std::endl;
  json bd_json1 = {
                   {"id", "gasnode1"},
                   {"type", "flow"},
                   {"data", json::array({{{"time", 0.},
                                          {"values", json::array({flow1start})}},{{"time", 100.},{"values", json::array({flow1end})}}})}};
  //std::cout << bd_json1<<std::endl;

  double pressure_start = 810;
  double pressure_end = 125;
  double flow_start =-4;
  double flow_end = 1000;
  json sp0_initial = {
                     {"id", "node_4_ld1"},
                     {"data", json::array({{{"x", 0.0},
                                            {"value", json::array({pressure_start,flow_start})}},{{"x", 1.0},{"value", json::array({pressure_end,flow_end})}}})}};
  //std::cout << sp0_initial<<std::endl;


  Model::Networkproblem::Gas::Flowboundarynode  g0("gasnode0", bd_json0, flow_topology);
  Model::Networkproblem::Gas::Flowboundarynode  g1("gasnode1", bd_json1, flow_topology);
  Model::Networkproblem::Gas::Shortpipe sp0("SP0", &g0, &g1);

  auto a = g0.set_indices(0);
  auto b = g1.set_indices(a);
  auto c = sp0.set_indices(b);

  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(c);
  rootvalues.setZero();
  Eigen::VectorXd last_state(c);
  Eigen::VectorXd new_state(c);

  sp0.set_initial_values(new_state,sp0_initial);
  // std::cout << "Initial conditions:" << std::endl;
  // std::cout << new_state <<std::endl;

  g0.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation  g0:" << std::endl;
  // std::cout << rootvalues << std::endl;


  g1.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation  g1:" << std::endl;
  // std::cout << rootvalues << std::endl;

  sp0.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation shortpipe:" << std::endl;
  // std::cout << rootvalues << std::endl;

  EXPECT_DOUBLE_EQ(rootvalues[0], flow_start-flow0start );
  EXPECT_DOUBLE_EQ(rootvalues[3], -flow_end-flow1start );
  EXPECT_DOUBLE_EQ(rootvalues[1], pressure_start-pressure_end );
  EXPECT_DOUBLE_EQ(rootvalues[2], flow_start-flow_end );

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  g0.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  g1.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  sp0.evaluate_state_derivative(&handler, last_time, new_time, last_state,new_state);
  handler.set_matrix();

  // std::cout << J;

  EXPECT_EQ(J.nonZeros(),8);

  Eigen::MatrixXd expected_J(new_state.size(), new_state.size());

  expected_J <<  //
    0, 1,  0,  0, //
    1, 0, -1,  0, //
    0, 1,  0, -1, //
    0, 0,  0, -1 ;//

  Eigen::SparseMatrix<double> sparse_expected = expected_J.sparseView();

  Eigen::SparseMatrix<double> difference = J-sparse_expected;

  auto max = difference.coeffs().maxCoeff();
  EXPECT_DOUBLE_EQ(max, 0.0);
}


TEST(testFlowboundarynode_Shortpipe, Flowboundarynode_multiple_shortpipes) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow2start = -23.0;
  double flow2end = -440.0;


  json flow_topology={};

  json bd_json0 = {
                   {"id", "gasnode0"},
                   {"type", "flow"},
                   {"data", json::array({{{"time", 0.},
                                          {"values", json::array({flow0start})}},{{"time", 100.},{"values", json::array({flow0end})}}})}};

  //std::cout << bd_json0 <<std::endl;

  json bd_json2 = {
                   {"id", "gasnode2"},
                   {"type", "flow"},
                   {"data", json::array({{{"time", 0.},
                                          {"values", json::array({flow2start})}},{{"time", 100.},{"values", json::array({flow2end})}}})}};
  //std::cout << bd_json1<<std::endl;



  double sp0_pressure_start = 810;
  double sp0_flow_start = -4;
  double sp0_pressure_end = 125;
  double sp0_flow_end = 1000;
  json sp0_initial = {
      {"id", "node_4_ld1"},
      {"data",
       json::array(
           {{{"x", 0.0},
             {"value", json::array({sp0_pressure_start, sp0_flow_start})}},
            {{"x", 1.0},
             {"value", json::array({sp0_pressure_end, sp0_flow_end})}}})}};
  //std::cout << sp0_initial<<std::endl;


  double sp1_pressure_start = 811;
  double sp1_flow_start = -8;
  double sp1_pressure_end = 131;
  double sp1_flow_end = 1111;

  json sp1_initial = {
      {"id", "node_4_ld1"},
      {"data",
       json::array(
           {{{"x", 0.0},
             {"value", json::array({sp1_pressure_start, sp1_flow_start})}},
            {{"x", 1.0},
             {"value", json::array({sp1_pressure_end, sp1_flow_end})}}})}};
  //std::cout << sp1_initial<<std::endl;



  Model::Networkproblem::Gas::Flowboundarynode g0("gasnode0", bd_json0, flow_topology);
  Model::Networkproblem::Gas::Innode g1("innode1");
  Model::Networkproblem::Gas::Flowboundarynode g2("gasnode2", bd_json2, flow_topology);
  Model::Networkproblem::Gas::Shortpipe sp0("SP0", &g0, &g1);
  Model::Networkproblem::Gas::Shortpipe sp1("SP1", &g1, &g2);

  auto a = g0.set_indices(0);
  auto b = g1.set_indices(a);
  auto c = g2.set_indices(b);
  auto d = sp0.set_indices(c);
  auto e = sp1.set_indices(d);
  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(e);
  rootvalues.setZero();
  Eigen::VectorXd last_state(e);
  Eigen::VectorXd new_state(e);

  sp0.set_initial_values(new_state, sp0_initial);
  // std::cout << "Initial conditions sp0:" << std::endl;
  // std::cout << new_state <<std::endl;

  sp1.set_initial_values(new_state, sp1_initial);
  // std::cout << "Initial conditions sp1:" << std::endl;
  // std::cout << new_state <<std::endl;

  g0.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation  g0:" << std::endl;
  // std::cout << rootvalues << std::endl;


  g1.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation  g1:" << std::endl;
  // std::cout << rootvalues << std::endl;

  g2.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation  g2:" << std::endl;
  // std::cout << rootvalues << std::endl;


  sp0.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation shortpipe0:" << std::endl;
  // std::cout << rootvalues << std::endl;

  sp1.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation shortpipe1:" << std::endl;
  // std::cout << rootvalues << std::endl;


  // inner node (multiple pipes connected)
  EXPECT_DOUBLE_EQ(rootvalues[4], sp0_pressure_end-sp1_pressure_start );
  EXPECT_DOUBLE_EQ(rootvalues[3], sp1_flow_start - sp0_flow_end );



  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  J.setZero();
  Aux::Triplethandler handler(&J);

  g0.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  g1.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  g2.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  sp0.evaluate_state_derivative(&handler, last_time, new_time, last_state,new_state);
  sp1.evaluate_state_derivative(&handler, last_time, new_time, last_state,new_state);

  handler.set_matrix();

  // std::cout << J;

  EXPECT_EQ(J.nonZeros(),20);
}


TEST(testFlowboundarynode_Shortpipe, Flowboundarynode_three_shortpipes) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow3start = -723.0;
  double flow3end = -640.0;

  double flow2start = -23.0;
  double flow2end = -440.0;


  json flow_topology={};

  json bd_json0 = {
                   {"id", "gasnode0"},
                   {"type", "flow"},
                   {"data", json::array({{{"time", 0.},
                                          {"values", json::array({flow0start})}},{{"time", 100.},{"values", json::array({flow0end})}}})}};

  //std::cout << bd_json0 <<std::endl;

  json bd_json2 = {
                   {"id", "gasnode2"},
                   {"type", "flow"},
                   {"data", json::array({{{"time", 0.},
                                          {"values", json::array({flow2start})}},{{"time", 100.},{"values", json::array({flow2end})}}})}};
  //std::cout << bd_json1<<std::endl;

  json bd_json3 = {
                   {"id", "gasnode3"},
                   {"type", "flow"},
                   {"data", json::array({{{"time", 0.},
                                          {"values", json::array({flow3start})}},{{"time", 100.},{"values", json::array({flow3end})}}})}};
  //std::cout << bd_json1<<std::endl;

 

  double sp0_pressure_start = 810;
  double sp0_flow_start = -4;
  double sp0_pressure_end = 125;
  double sp0_flow_end = 1000;
  json sp0_initial = {
      {"id", "node_4_ld1"},
      {"data",
       json::array(
           {{{"x", 0.0},
             {"value", json::array({sp0_pressure_start, sp0_flow_start})}},
            {{"x", 1.0},
             {"value", json::array({sp0_pressure_end, sp0_flow_end})}}})}};
  //std::cout << sp0_initial<<std::endl;


  double sp1_pressure_start = 811;
  double sp1_flow_start = -8;
  double sp1_pressure_end = 131;
  double sp1_flow_end = 1111;

  json sp1_initial = {
      {"id", "node_4_ld1"},
      {"data",
       json::array(
           {{{"x", 0.0},
             {"value", json::array({sp1_pressure_start, sp1_flow_start})}},
            {{"x", 1.0},
             {"value", json::array({sp1_pressure_end, sp1_flow_end})}}})}};
  //std::cout << sp1_initial<<std::endl;

  double sp2_pressure_start = 822;
  double sp2_flow_start = -20;
  double sp2_pressure_end = 232;
  double sp2_flow_end = 2222;

  json sp2_initial = {
                      {"id", "node_4_ld2"},
                      {"data",
                       json::array(
                                   {{{"x", 0.0},
                                     {"value", json::array({sp2_pressure_start, sp2_flow_start})}},
                                    {{"x", 1.0},
                                     {"value", json::array({sp2_pressure_end, sp2_flow_end})}}})}};
  //std::cout << sp2_initial<<std::endl;


  Model::Networkproblem::Gas::Flowboundarynode g0("gasnode0", bd_json0, flow_topology);
  Model::Networkproblem::Gas::Innode g1("innode1");
  Model::Networkproblem::Gas::Flowboundarynode g2("gasnode2", bd_json2, flow_topology);
  Model::Networkproblem::Gas::Pressureboundarynode g3("gasnode3", bd_json3, flow_topology);
  Model::Networkproblem::Gas::Shortpipe sp0("SP0", &g0, &g1);
  Model::Networkproblem::Gas::Shortpipe sp1("SP1", &g1, &g2);
  Model::Networkproblem::Gas::Shortpipe sp2("SP1", &g1, &g3);

  auto a = g0.set_indices(0);
  auto b = g1.set_indices(a);
  auto c = g2.set_indices(b);
  auto c2 = g3.set_indices(c);
  auto d = sp0.set_indices(c2);
  auto f = sp1.set_indices(d);
  auto e = sp2.set_indices(f);
  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(e);
  rootvalues.setZero();
  Eigen::VectorXd last_state(e);
  Eigen::VectorXd new_state(e);

  sp0.set_initial_values(new_state, sp0_initial);
  // std::cout << "Initial conditions sp0:" << std::endl;
  // std::cout << new_state <<std::endl;

  sp1.set_initial_values(new_state, sp1_initial);
  // std::cout << "Initial conditions sp1:" << std::endl;
  // std::cout << new_state <<std::endl;

  sp2.set_initial_values(new_state, sp2_initial);
  // std::cout << "Initial conditions sp2:" << std::endl;
  // std::cout << new_state <<std::endl;


  g0.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation  g0:" << std::endl;
  // std::cout << rootvalues << std::endl;


  g1.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation  g1:" << std::endl;
  // std::cout << rootvalues << std::endl;

  g2.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation  g2:" << std::endl;
  // std::cout << rootvalues << std::endl;

  g3.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation  g3:" << std::endl;
  // std::cout << rootvalues << std::endl;



  sp0.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation shortpipe0:" << std::endl;
  // std::cout << rootvalues << std::endl;

  sp1.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation shortpipe1:" << std::endl;
  // std::cout << rootvalues << std::endl;

  sp2.evaluate(rootvalues, last_time, new_time, last_state, new_state);
  // std::cout << "Evaluation shortpipe2:" << std::endl;
  // std::cout << rootvalues << std::endl;

  // inner node (multiple pipes connected)
  

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  J.setZero();
  Aux::Triplethandler handler(&J);

  g0.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  g1.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  g2.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  g3.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  sp0.evaluate_state_derivative(&handler, last_time, new_time, last_state,new_state);
  sp1.evaluate_state_derivative(&handler, last_time, new_time, last_state,new_state);
  sp2.evaluate_state_derivative(&handler, last_time, new_time, last_state,new_state);

  handler.set_matrix();

  // std::cout << J << std::endl;

  // shortpipes: 3 * 4
  // outer nodes: 6
  // inner node flow: 6
  // inner node pressure: 8
  EXPECT_EQ(J.nonZeros(),3*4+6+6+8);

  Eigen::MatrixXd jd = J;
  // std::cout << jd.inverse().sparseView()<< std::endl;
  std::cout <<  "\033[1;31mMaybe also test evaluate in this multi-edge setting.\033[0m\n" <<std::endl;
}


TEST(testPipe, evaluate) {
  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow1start = -23.0;
  double flow1end = -440.0;

  json flow_topology={};

  json bd_json0 = {
                   {"id", "gasnode0"},
                   {"type", "flow"},
                   {"data", json::array({{{"time", 0.},
                                          {"values", json::array({flow0start})}},{{"time", 100.},{"values", json::array({flow0end})}}})}};

  //std::cout << bd_json0 <<std::endl;
  json bd_json1 = {
                   {"id", "gasnode1"},
                   {"type", "flow"},
                   {"data", json::array({{{"time", 0.},
                                          {"values", json::array({flow1start})}},{{"time", 100.},{"values", json::array({flow1end})}}})}};
  //std::cout << bd_json1<<std::endl;

  double pressure_start = 810;
  double pressure_end = 125;
  double flow_start =-4;
  double flow_end = 1000;
  json p0_initial = {
                      {"id", "node_4_ld1"},
                      {"data", json::array({{{"x", 0.0},
                                             {"value", json::array({pressure_start,flow_start})}},{{"x", 1.0},{"value", json::array({pressure_end,flow_end})}}})}};
  //std::cout << sp0_initial<<std::endl;

  std::string pipe_topology_string =
    "{\n"
    "\"id\": \"p_br2\",\n"
    "\"from\": \"node_2\",\n"
    "\"to\": \"node_3\",\n"
    "\"heatTransferCoefficient\": \"2\",\n"
    "\"flowMin\": {\n"
    "\"unit\": \"m_cube_per_s\",\n"
    "\"value\": \"-6369.426752\"\n"
    "},\n"
    "\"flowMax\": {\n"
    "\"unit\": \"m_cube_per_s\",\n"
    "\"value\": \"6369.426752\"\n"
    "},\n"
    "\"flowInInit\": {\n"
    "\"unit\": \"m_cube_per_s\",\n"
    "\"value\": \"-6369.426752\"\n"
    "},\n"
    "\"flowOutInit\": {\n"
    "\"unit\": \"m_cube_per_s\",\n"
    "\"value\": \"-6369.426752\"\n"
    "},\n"
    "\"length\": {\n"
    "\"unit\": \"km\",\n"
    "\"value\": \"15.25\"\n"
    "},\n"
    "\"diameter\": {\n"
    "\"unit\": \"mm\",\n"
    "\"value\": \"914.400000\"\n"
    "},\n"
    "\"roughness\": {\n"
    "\"unit\": \"m\",\n"
    "\"value\": \"0.000008\"\n"
    "}\n"
    "}";
  json pipe_topology = json::parse(pipe_topology_string);

  std::string pipe_initial_string ="{\n"
    " \"id\": \"p_br2\",\n"
    " \"data\": [\n"
    "{\n"
    "  \"x\": 0.0,\n"
    "    \"value\": [\n"
    "              75.046978,\n"
    "              58.290215\n"
    "              ]\n"
    "    },\n"
    "{\n"
    "  \"x\": 3812.5,\n"
    "    \"value\": [\n"
    "              75.032557,\n"
    "              58.105963\n"
    "              ]\n"
    "    },\n"
    "{\n"
    "  \"x\": 7625.0,\n"
    "    \"value\": [\n"
    "              75.01822,\n"
    "              57.921692\n"
    "              ]\n"
    "    },\n"
    "{\n"
    "  \"x\": 11437.5,\n"
    "    \"value\": [\n"
    "              75.003966,\n"
    "              57.737405\n"
    "              ]\n"
    "    },\n"
    "{\n"
    "  \"x\": 15250.0,\n"
    "    \"value\": [\n"
    "              74.989795,\n"
    "              57.553105\n"
    "              ]\n"
    "    }\n"
    "          ]\n"
    "}";

  json pipe_initial = json::parse(pipe_initial_string);
  double Delta_x = 20000;

  Model::Networkproblem::Gas::Flowboundarynode  g0("gasnode0", bd_json0, flow_topology);
  Model::Networkproblem::Gas::Flowboundarynode  g1("gasnode1", bd_json1, flow_topology);
  Model::Networkproblem::Gas::Pipe p0("P0", &g0, &g1, pipe_topology, Delta_x);
  // p0.display();

  auto a = g0.set_indices(0);
  auto b = g1.set_indices(a);
  auto c = p0.set_indices(b);
  
  double last_time = 0.0;
  double new_time = 10.0;
  Eigen::VectorXd rootvalues(c);
  rootvalues.setZero();
  Eigen::VectorXd last_state(c);
  Eigen::VectorXd new_state(c);

  p0.set_initial_values(new_state,pipe_initial);
  last_state = new_state;
  // std::cout << "Initial conditions:" << std::endl;
  // std::cout << new_state <<std::endl;

  // std::cout << p0.get_boundary_p_qvol_bar(1,new_state) <<std::endl;
  // std::cout << p0.get_boundary_p_qvol_bar(-1,new_state) <<std::endl;

  p0.evaluate(rootvalues, last_time, new_time, last_state, new_state);

  

  double Delta_t = new_time-last_time;
  Eigen::Vector2d last_left = last_state.segment<2>(0);
  Eigen::Vector2d last_right = last_state.segment<2>(2);

  Eigen::Vector2d new_left = new_state.segment<2>(0);
  Eigen::Vector2d new_right = new_state.segment<2>(2);

  Eigen::Vector2d flux_right = p0.bl.flux(new_right);
  Eigen::Vector2d flux_left = p0.bl.flux(new_left);

  Eigen::Vector2d source_right = p0.bl.source(new_right);
  Eigen::Vector2d source_left = p0.bl.source(new_left);


  Eigen::Vector2d expected_result = 0.5*(new_left +new_right) - 0.5*(last_left+last_right) +Delta_t/Delta_x*(flux_right -flux_left) - Delta_t* (source_left-source_right);


  std::cout << expected_result << std::endl;
  std::cout << rootvalues.segment<2>(1) <<std::endl;

  }
