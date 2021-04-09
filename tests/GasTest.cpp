#include "Equationcomponent_test_helpers.hpp"
#include "Flowboundarynode.hpp"
#include "Gas_factory.hpp"
#include "Implicitboxscheme.hpp"
#include "Innode.hpp"
#include "Isothermaleulerequation.hpp"
#include "Matrixhandler.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "Pipe.hpp"
#include "Shortpipe.hpp"
#include "test_io_helper.hpp"
#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

class GasTEST : public ::testing::Test {
public:
  std::string output;

  Directory_creator d;
  Path_changer p{d.get_path()};

  std::unique_ptr<Model::Networkproblem::Networkproblem>
  get_Networkproblem(nlohmann::json &netproblem) {
    std::unique_ptr<Model::Networkproblem::Networkproblem> netprob;
    {
      std::stringstream buffer;
      Catch_cout catcher(buffer.rdbuf());
      Model::Componentfactory::Gas_factory factory;
      auto net_ptr = Model::Networkproblem::build_net(
          netproblem, factory, std::filesystem::current_path());
      netprob = std::make_unique<Model::Networkproblem::Networkproblem>(
          std::move(net_ptr));
      output = buffer.str();
    }

    return netprob;
  }
};

nlohmann::json source_json(std::string id, double flowstart, double flowend);

nlohmann::json sink_json(std::string id, double flowstart, double flowend);

nlohmann::json shortpipe_json(
    std::string id, nlohmann::json startnode, nlohmann::json endnode);

nlohmann::json pipe_json(
    std::string id, nlohmann::json startnode, nlohmann::json endnode,
    double length, std::string length_unit, double diameter,
    std::string diameter_unit, double roughness, std::string roughness_unit,
    double desired_delta_x);

TEST_F(GasTEST, Shortpipe_evaluate) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow1start = -23.0;
  double flow1end = -440.0;

  auto node0_topology = source_json("node0", flow0start, flow0end);
  auto node1_topology = source_json("node1", flow1start, flow1end);
  auto shortpipe_topology
      = shortpipe_json("shortpipe", node0_topology, node1_topology);
  auto np_json = make_full_json(
      {{"Source", {node0_topology, node1_topology}}},
      {{"Shortpipe", {shortpipe_topology}}});

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

  nlohmann::json np_initialjson
      = make_initial_json({}, {{"Shortpipe", {initial_json}}});
  ;

  netprob->set_initial_values(last_state, np_initialjson);
  Eigen::VectorXd new_state = last_state;
  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);

  EXPECT_DOUBLE_EQ(rootvalues[1], pressure_start - pressure_end);
  EXPECT_DOUBLE_EQ(rootvalues[2], flow_start - flow_end);
}

TEST_F(GasTEST, Shortpipe_evaluate_state_derivative) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow1start = -23.0;
  double flow1end = -440.0;

  auto node0_topology = source_json("node0", flow0start, flow0end);
  auto node1_topology = source_json("node1", flow1start, flow1end);
  auto shortpipe_topology
      = shortpipe_json("shortpipe", node0_topology, node1_topology);
  auto np_json = make_full_json(
      {{"Source", {node0_topology, node1_topology}}},
      {{"Shortpipe", {shortpipe_topology}}});

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

  nlohmann::json np_initialjson
      = make_initial_json({}, {{"Shortpipe", {initial_json}}});

  netprob->set_initial_values(last_state, np_initialjson);
  Eigen::VectorXd new_state = last_state;
  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  netprob->evaluate_state_derivative(
      &handler, last_time, new_time, last_state, new_state);
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

  auto shortpipe01_json = shortpipe_json("shortpipe01", node0_json, node1_json);
  auto shortpipe20_json = shortpipe_json("shortpipe20", node2_json, node0_json);

  auto np_json = make_full_json(
      {{"Source", {node0_json, node1_json, node2_json}}},
      {{"Shortpipe", {shortpipe01_json, shortpipe20_json}}});

  auto netprob = get_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  double sp01_pressure_start = 810;
  double sp01_flow_start = -4;
  double sp01_pressure_end = 125;
  double sp01_flow_end = 1000;

  nlohmann::json initial01;
  {
    Eigen::Vector2d cond0(sp01_pressure_start, sp01_flow_start);
    Eigen::Vector2d cond1(sp01_pressure_end, sp01_flow_end);
    initial01 = make_value_json("shortpipe01", "x", cond0, cond1);
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

  auto initial_json
      = make_initial_json({}, {{"Shortpipe", {initial01, initial20}}});

  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);

  netprob->set_initial_values(new_state, initial_json);

  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);

  // node0:
  EXPECT_DOUBLE_EQ(rootvalues[0], -sp01_pressure_start + sp20_pressure_end);
  EXPECT_DOUBLE_EQ(
      rootvalues[7], -flow0start + sp01_flow_start - sp20_flow_end);

  // node 1:
  EXPECT_DOUBLE_EQ(rootvalues[3], -flow1start - sp01_flow_end);

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

  auto shortpipe01_json = shortpipe_json("shortpipe01", node0_json, node1_json);
  auto shortpipe20_json = shortpipe_json("shortpipe20", node2_json, node0_json);

  auto np_json = make_full_json(
      {{"Source", {node0_json, node1_json, node2_json}}},
      {{"Shortpipe", {shortpipe01_json, shortpipe20_json}}});

  auto netprob = get_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  double sp01_pressure_start = 810;
  double sp01_flow_start = -4;
  double sp01_pressure_end = 125;
  double sp01_flow_end = 1000;

  nlohmann::json initial01;
  {
    Eigen::Vector2d cond0(sp01_pressure_start, sp01_flow_start);
    Eigen::Vector2d cond1(sp01_pressure_end, sp01_flow_end);
    initial01 = make_value_json("shortpipe01", "x", cond0, cond1);
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
  auto initial_json
      = make_initial_json({}, {{"Shortpipe", {initial01, initial20}}});

  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);

  netprob->set_initial_values(new_state, initial_json);

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  netprob->evaluate_state_derivative(
      &handler, last_time, new_time, last_state, new_state);
  handler.set_matrix();

  Eigen::MatrixXd DenseJ = J;

  // node0:
  EXPECT_DOUBLE_EQ(DenseJ(0, 0), -1);
  EXPECT_DOUBLE_EQ(DenseJ(0, 6), 1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 0 or i == 6) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(0, i), 0.0);
  }

  EXPECT_DOUBLE_EQ(DenseJ(7, 1), 1);
  EXPECT_DOUBLE_EQ(DenseJ(7, 7), -1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 1 or i == 7) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(7, i), 0.0);
  }

  // node1:
  EXPECT_DOUBLE_EQ(DenseJ(3, 3), -1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 3) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(3, i), 0.0);
  }

  // node2:
  EXPECT_DOUBLE_EQ(DenseJ(4, 5), 1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 5) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(4, i), 0.0);
  }
}

TEST_F(GasTEST, Sink_evaluate) {

  double flow0start = 88.0;
  double flow0end = 10.0;

  double flow1start = -23.0;
  double flow1end = -440.0;

  double flow2start = -383.0;
  double flow2end = 81.0;

  auto node0_json = sink_json("node0", flow0start, flow0end);
  auto node1_json = sink_json("node1", flow1start, flow1end);
  auto node2_json = sink_json("node2", flow2start, flow2end);

  auto shortpipe01_json = shortpipe_json("shortpipe01", node0_json, node1_json);
  auto shortpipe20_json = shortpipe_json("shortpipe20", node2_json, node0_json);

  auto np_json = make_full_json(
      {{"Sink", {node0_json, node1_json, node2_json}}},
      {{"Shortpipe", {shortpipe01_json, shortpipe20_json}}});

  auto netprob = get_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  double sp01_pressure_start = 810;
  double sp01_flow_start = -4;
  double sp01_pressure_end = 125;
  double sp01_flow_end = 1000;

  nlohmann::json initial01;
  {
    Eigen::Vector2d cond0(sp01_pressure_start, sp01_flow_start);
    Eigen::Vector2d cond1(sp01_pressure_end, sp01_flow_end);
    initial01 = make_value_json("shortpipe01", "x", cond0, cond1);
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

  auto initial_json
      = make_initial_json({}, {{"Shortpipe", {initial01, initial20}}});

  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);

  netprob->set_initial_values(new_state, initial_json);

  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);

  // Note that for sinks the boundary conditions should have the opposite signs
  // compared to sources. node0:
  EXPECT_DOUBLE_EQ(rootvalues[0], -sp01_pressure_start + sp20_pressure_end);
  EXPECT_DOUBLE_EQ(rootvalues[7], flow0start + sp01_flow_start - sp20_flow_end);

  // node 1:
  EXPECT_DOUBLE_EQ(rootvalues[3], flow1start - sp01_flow_end);

  // node 2:
  EXPECT_DOUBLE_EQ(rootvalues[4], flow2start + sp20_flow_start);
}

TEST_F(GasTEST, Sink_evaluate_state_derivative) {

  double flow0start = 86.0;
  double flow0end = 193.0;

  double flow1start = -23.0;
  double flow1end = -499.0;

  double flow2start = -983.0;
  double flow2end = 8000.0;

  auto node0_json = sink_json("node0", flow0start, flow0end);
  auto node1_json = sink_json("node1", flow1start, flow1end);
  auto node2_json = sink_json("node2", flow2start, flow2end);

  auto shortpipe01_json = shortpipe_json("shortpipe01", node0_json, node1_json);
  auto shortpipe20_json = shortpipe_json("shortpipe20", node2_json, node0_json);

  auto np_json = make_full_json(
      {{"Sink", {node0_json, node1_json, node2_json}}},
      {{"Shortpipe", {shortpipe01_json, shortpipe20_json}}});

  auto netprob = get_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  double sp01_pressure_start = 810;
  double sp01_flow_start = -4;
  double sp01_pressure_end = 125;
  double sp01_flow_end = 1000;

  nlohmann::json initial01;
  {
    Eigen::Vector2d cond0(sp01_pressure_start, sp01_flow_start);
    Eigen::Vector2d cond1(sp01_pressure_end, sp01_flow_end);
    initial01 = make_value_json("shortpipe01", "x", cond0, cond1);
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
  auto initial_json
      = make_initial_json({}, {{"Shortpipe", {initial01, initial20}}});

  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);

  netprob->set_initial_values(new_state, initial_json);

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  netprob->evaluate_state_derivative(
      &handler, last_time, new_time, last_state, new_state);
  handler.set_matrix();

  Eigen::MatrixXd DenseJ = J;

  // node0:
  EXPECT_DOUBLE_EQ(DenseJ(0, 0), -1);
  EXPECT_DOUBLE_EQ(DenseJ(0, 6), 1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 0 or i == 6) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(0, i), 0.0);
  }

  EXPECT_DOUBLE_EQ(DenseJ(7, 1), 1);
  EXPECT_DOUBLE_EQ(DenseJ(7, 7), -1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 1 or i == 7) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(7, i), 0.0);
  }

  // node1:
  EXPECT_DOUBLE_EQ(DenseJ(3, 3), -1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 3) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(3, i), 0.0);
  }

  // node2:
  EXPECT_DOUBLE_EQ(DenseJ(4, 5), 1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 5) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(4, i), 0.0);
  }
}

TEST_F(GasTEST, Innode_evaluate) {

  nlohmann::json node0_json;
  nlohmann::json node1_json;
  nlohmann::json node2_json;
  node0_json["id"] = "node0";
  node1_json["id"] = "node1";
  node2_json["id"] = "node2";

  auto shortpipe01_json = shortpipe_json("shortpipe01", node0_json, node1_json);
  auto shortpipe20_json = shortpipe_json("shortpipe20", node2_json, node0_json);

  auto np_json = make_full_json(
      {{"Innode", {node0_json, node1_json, node2_json}}},
      {{"Shortpipe", {shortpipe01_json, shortpipe20_json}}});

  auto netprob = get_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  double sp01_pressure_start = 8102;
  double sp01_flow_start = -49;
  double sp01_pressure_end = 1257;
  double sp01_flow_end = 10001;

  nlohmann::json initial01;
  {
    Eigen::Vector2d cond0(sp01_pressure_start, sp01_flow_start);
    Eigen::Vector2d cond1(sp01_pressure_end, sp01_flow_end);
    initial01 = make_value_json("shortpipe01", "x", cond0, cond1);
  }
  double sp20_pressure_start = 8110;
  double sp20_flow_start = -83;
  double sp20_pressure_end = 1315;
  double sp20_flow_end = 11118;

  nlohmann::json initial20;
  {
    Eigen::Vector2d cond0(sp20_pressure_start, sp20_flow_start);
    Eigen::Vector2d cond1(sp20_pressure_end, sp20_flow_end);
    initial20 = make_value_json("shortpipe20", "x", cond0, cond1);
  }

  auto initial_json
      = make_initial_json({}, {{"Shortpipe", {initial01, initial20}}});

  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);

  netprob->set_initial_values(new_state, initial_json);

  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);

  // node0:
  EXPECT_DOUBLE_EQ(rootvalues[0], -sp01_pressure_start + sp20_pressure_end);
  EXPECT_DOUBLE_EQ(rootvalues[7], sp01_flow_start - sp20_flow_end);

  // node 1:
  EXPECT_DOUBLE_EQ(rootvalues[3], -sp01_flow_end);

  // node 2:
  EXPECT_DOUBLE_EQ(rootvalues[4], sp20_flow_start);
}

TEST_F(GasTEST, Innode_evaluate_state_derivative) {

  nlohmann::json node0_json;
  nlohmann::json node1_json;
  nlohmann::json node2_json;
  node0_json["id"] = "node0";
  node1_json["id"] = "node1";
  node2_json["id"] = "node2";

  auto shortpipe01_json = shortpipe_json("shortpipe01", node0_json, node1_json);
  auto shortpipe20_json = shortpipe_json("shortpipe20", node2_json, node0_json);

  auto np_json = make_full_json(
      {{"Innode", {node0_json, node1_json, node2_json}}},
      {{"Shortpipe", {shortpipe01_json, shortpipe20_json}}});

  auto netprob = get_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  double sp01_pressure_start = 81000;
  double sp01_flow_start = -411;
  double sp01_pressure_end = 12522;
  double sp01_flow_end = 100033;

  nlohmann::json initial01;
  {
    Eigen::Vector2d cond0(sp01_pressure_start, sp01_flow_start);
    Eigen::Vector2d cond1(sp01_pressure_end, sp01_flow_end);
    initial01 = make_value_json("shortpipe01", "x", cond0, cond1);
  }
  double sp20_pressure_start = 81144;
  double sp20_flow_start = -855;
  double sp20_pressure_end = 13166;
  double sp20_flow_end = 111177;

  nlohmann::json initial20;
  {
    Eigen::Vector2d cond0(sp20_pressure_start, sp20_flow_start);
    Eigen::Vector2d cond1(sp20_pressure_end, sp20_flow_end);
    initial20 = make_value_json("shortpipe20", "x", cond0, cond1);
  }
  auto initial_json
      = make_initial_json({}, {{"Shortpipe", {initial01, initial20}}});

  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);

  netprob->set_initial_values(new_state, initial_json);

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  netprob->evaluate_state_derivative(
      &handler, last_time, new_time, last_state, new_state);
  handler.set_matrix();

  Eigen::MatrixXd DenseJ = J;

  // node0:
  EXPECT_DOUBLE_EQ(DenseJ(0, 0), -1);
  EXPECT_DOUBLE_EQ(DenseJ(0, 6), 1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 0 or i == 6) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(0, i), 0.0);
  }

  EXPECT_DOUBLE_EQ(DenseJ(7, 1), 1);
  EXPECT_DOUBLE_EQ(DenseJ(7, 7), -1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 1 or i == 7) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(7, i), 0.0);
  }

  // node1:
  EXPECT_DOUBLE_EQ(DenseJ(3, 3), -1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 3) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(3, i), 0.0);
  }

  // node2:
  EXPECT_DOUBLE_EQ(DenseJ(4, 5), 1);
  for (Eigen::Index i = 0; i != 8; ++i) {
    if (i == 5) {
      continue;
    }
    EXPECT_DOUBLE_EQ(DenseJ(4, i), 0.0);
  }
}

TEST_F(GasTEST, Pipe_evaluate) {

  nlohmann::json node0;
  nlohmann::json node1;
  node0["id"] = "node0";
  node1["id"] = "node1";

  std::string id = "pipe";
  double length = 15250;
  double diameter = 0.9144;
  double roughness = 8e-6;
  double desired_delta_x = 20000;

  nlohmann::json pipe_topology = pipe_json(
      id, node0, node1, length, "m", diameter, "m", roughness, "m",
      desired_delta_x);

  std::vector<std::pair<double, Eigen::Matrix<double, 2, 1>>> initialvalues;
  using E2d = Eigen::Matrix<double, 2, 1>;
  double initial_Delta_x = 3812.5;
  initialvalues.push_back({0, E2d(75.046978, 58.290215)});
  initialvalues.push_back({initial_Delta_x, E2d(75.032557, 58.105963)});
  initialvalues.push_back({2 * initial_Delta_x, E2d(75.01822, 57.921692)});
  initialvalues.push_back({3 * initial_Delta_x, E2d(75.003966, 57.737405)});
  initialvalues.push_back({4 * initial_Delta_x, E2d(74.989795, 57.553105)});

  nlohmann::json pipe_initial = make_value_json(id, "x", initialvalues);

  nlohmann::json net_initial
      = make_initial_json({}, {{"Pipe", {pipe_initial}}});

  auto netprop_json = make_full_json(
      {{"Innode", {node0, node1}}}, {{"Pipe", {pipe_topology}}});

  auto netprob = get_Networkproblem(netprop_json);

  int number_of_variables = netprob->set_indices(0);

  double last_time = 0.0;
  double new_time = 10.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);

  netprob->set_initial_values(new_state, net_initial);

  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);

  Eigen::Vector2d last_left = last_state.segment<2>(0);
  Eigen::Vector2d last_right = last_state.segment<2>(2);

  Eigen::Vector2d new_left = new_state.segment<2>(0);
  Eigen::Vector2d new_right = new_state.segment<2>(2);

  Eigen::Vector2d expected_result;

  double actual_Delta_x = length;

  Model::Balancelaw::Isothermaleulerequation bl;
  Model::Scheme::Implicitboxscheme scheme;
  scheme.evaluate_point(
      expected_result, last_time, new_time, actual_Delta_x, last_left,
      last_right, new_left, new_right, bl, diameter, roughness);

  EXPECT_DOUBLE_EQ(expected_result[0], rootvalues.segment<2>(1)[0]);
  EXPECT_DOUBLE_EQ(expected_result[1], rootvalues.segment<2>(1)[1]);
}

TEST_F(GasTEST, Pipe_evaluate_state_derivative) {
  nlohmann::json node0;
  nlohmann::json node1;
  node0["id"] = "node0";
  node1["id"] = "node1";

  std::string id = "pipe";
  double length = 15250;
  double diameter = 0.9144;
  double roughness = 8e-6;
  double desired_delta_x = 20000;

  nlohmann::json pipe_topology = pipe_json(
      id, node0, node1, length, "m", diameter, "m", roughness, "m",
      desired_delta_x);

  std::vector<std::pair<double, Eigen::Matrix<double, 2, 1>>> initialvalues;
  {
    using E2d = Eigen::Matrix<double, 2, 1>;
    double initial_Delta_x = 3812.5;
    initialvalues.push_back({0, E2d(75.046978, 58.290215)});
    initialvalues.push_back({initial_Delta_x, E2d(75.032557, 58.105963)});
    initialvalues.push_back({2 * initial_Delta_x, E2d(75.01822, 57.921692)});
    initialvalues.push_back({3 * initial_Delta_x, E2d(75.003966, 57.737405)});
    initialvalues.push_back({4 * initial_Delta_x, E2d(74.989795, 57.553105)});
  }

  nlohmann::json pipe_initial = make_value_json(id, "x", initialvalues);
  nlohmann::json net_initial
      = make_initial_json({}, {{"Pipe", {pipe_initial}}});

  auto netprop_json = make_full_json(
      {{"Innode", {node0, node1}}}, {{"Pipe", {pipe_topology}}});

  auto netprob = get_Networkproblem(netprop_json);

  int number_of_variables = netprob->set_indices(0);

  double last_time = 0.0;
  double new_time = 10.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);
  last_state.setRandom();

  netprob->set_initial_values(new_state, net_initial);

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  netprob->evaluate_state_derivative(
      &handler, last_time, new_time, last_state, new_state);
  handler.set_matrix();

  Eigen::Matrix4d DenseJ = J;

  Model::Balancelaw::Isothermaleulerequation bl;
  Model::Scheme::Implicitboxscheme scheme;
  double Delta_x = length;
  auto last_left = last_state.segment<2>(0);
  auto last_right = last_state.segment<2>(2);
  auto new_left = new_state.segment<2>(0);
  auto new_right = new_state.segment<2>(2);

  auto dleft = scheme.devaluate_point_dleft(
      last_time, new_time, Delta_x, last_left, last_right, new_left, new_right,
      bl, diameter, roughness);
  auto dright = scheme.devaluate_point_dright(
      last_time, new_time, Delta_x, last_right, last_right, new_right,
      new_right, bl, diameter, roughness);

  EXPECT_DOUBLE_EQ(DenseJ(1, 0), dleft(0, 0));
  EXPECT_DOUBLE_EQ(DenseJ(1, 1), dleft(0, 1));
  EXPECT_DOUBLE_EQ(DenseJ(1, 2), dright(0, 0));
  EXPECT_DOUBLE_EQ(DenseJ(1, 3), dright(0, 1));

  EXPECT_DOUBLE_EQ(DenseJ(2, 0), dleft(1, 0));
  EXPECT_DOUBLE_EQ(DenseJ(2, 1), dleft(1, 1));
  EXPECT_DOUBLE_EQ(DenseJ(2, 2), dright(1, 0));
  EXPECT_DOUBLE_EQ(DenseJ(2, 3), dright(1, 1));
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
nlohmann::json shortpipe_json(
    std::string id, nlohmann::json startnode, nlohmann::json endnode) {

  nlohmann::json shortpipe_topology;
  shortpipe_topology["id"] = id;
  shortpipe_topology["from"] = startnode["id"];
  shortpipe_topology["to"] = endnode["id"];

  return shortpipe_topology;
}

nlohmann::json pipe_json(
    std::string id, nlohmann::json startnode, nlohmann::json endnode,
    double length, std::string length_unit, double diameter,
    std::string diameter_unit, double roughness, std::string roughness_unit,
    double desired_delta_x) {
  nlohmann::json pipe_topology;
  pipe_topology["id"] = id;
  pipe_topology["from"] = startnode["id"];
  pipe_topology["to"] = endnode["id"];
  pipe_topology["length"]["value"] = length;
  pipe_topology["length"]["unit"] = length_unit;
  pipe_topology["diameter"]["value"] = diameter;
  pipe_topology["diameter"]["unit"] = diameter_unit;
  pipe_topology["roughness"]["value"] = roughness;
  pipe_topology["roughness"]["unit"] = roughness_unit;
  pipe_topology["desired_delta_x"] = desired_delta_x;
  return pipe_topology;
}
