#include "Gaspowerconnection.hpp"
#include "Equationcomponent_test_helpers.hpp"
#include "ExternalPowerplant.hpp"
#include "Full_factory.hpp"
#include "Innode.hpp"
#include "Mathfunctions.hpp"
#include "Matrixhandler.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "Powernode.hpp"
#include "test_io_helper.hpp"
#include <Eigen/src/Core/Matrix.h>
#include <filesystem>
#include <gtest/gtest.h>
#include <stdexcept>

class GaspowerconnectionTEST : public ::testing::Test {
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
      Model::Componentfactory::Full_factory factory;
      nlohmann::json outputjson;
      auto net_ptr = Model::Networkproblem::build_net(
          netproblem, factory, std::filesystem::current_path(), outputjson);
      netprob = std::make_unique<Model::Networkproblem::Networkproblem>(
          std::move(net_ptr));
      output = buffer.str();
    }

    return netprob;
  }
};

nlohmann::json gaspowerconnection_json(
    std::string id, nlohmann::json startnode, nlohmann::json endnode,
    double gas2power_q_coefficient, double power2gas_q_coefficient);

nlohmann::json externalpowerplant_json(
    std::string id, double G, double B, Eigen::Vector2d cond0,
    Eigen::Vector2d cond1);

nlohmann::json gaspowerconnection_json(
    std::string id, nlohmann::json startnode, nlohmann::json endnode,
    double gas2power_q_coefficient, double power2gas_q_coefficient) {
  nlohmann::json gp_topology;
  gp_topology["id"] = id;
  gp_topology["from"] = startnode["id"];
  gp_topology["to"] = endnode["id"];
  gp_topology["gas2power_q_coeff"] = gas2power_q_coefficient;
  gp_topology["power2gas_q_coeff"] = power2gas_q_coefficient;
  Eigen::Matrix<double, 1, 1> cond0;
  cond0 << 1.0;
  Eigen::Matrix<double, 1, 1> cond1;
  cond1 << 1.0;
  auto bound = make_value_json(id, "time", cond0, cond1);
  gp_topology["boundary_values"] = bound;
  Eigen::Matrix<double, 1, 1> control0;
  control0 << 1.0;
  Eigen::Matrix<double, 1, 1> control1;
  control1 << 1.0;
  auto control = make_value_json(id, "time", control0, control1);
  gp_topology["control_values"] = control;

  return gp_topology;
}

nlohmann::json externalpowerplant_json(
    std::string id, double G, double B, Eigen::Vector2d cond0,
    Eigen::Vector2d cond1) {
  nlohmann::json extpowerplant_topology;
  extpowerplant_topology["id"] = id;
  extpowerplant_topology["G"] = G;
  extpowerplant_topology["B"] = B;
  auto bound = make_value_json(id, "time", cond0, cond1);
  extpowerplant_topology["boundary_values"] = bound;
  return extpowerplant_topology;
}

TEST_F(GaspowerconnectionTEST, smoothing_polynomial) {
  nlohmann::json innode_json;
  innode_json["id"] = "ainnode";
  double G = 4;
  double B = 123;
  Eigen::Vector2d cond0(345, 333);
  Eigen::Vector2d cond1(341, 3331);
  auto extpowerplant_json
      = externalpowerplant_json("bextpowerplant", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 55;

  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = get_Networkproblem(netprob_json);

  auto edges = netprob->get_network().get_edges();
  if (edges.size() != 1) {
    FAIL();
  }
  auto gp = dynamic_cast<
      Model::Networkproblem::Gaspowerconnection::Gaspowerconnection const *>(
      edges.front());
  if (not gp) {
    FAIL();
  }

  EXPECT_DOUBLE_EQ(gp->smoothing_polynomial(0), 0);
  EXPECT_DOUBLE_EQ(
      gp->smoothing_polynomial(gp->kappa), gas2power_q_coefficient * gp->kappa);
  EXPECT_DOUBLE_EQ(
      gp->smoothing_polynomial(-gp->kappa),
      power2gas_q_coefficient * (-gp->kappa));
  EXPECT_THROW(gp->smoothing_polynomial(gp->kappa + 1), std::runtime_error);
  EXPECT_THROW(gp->smoothing_polynomial(-gp->kappa - 1), std::runtime_error);
}

TEST_F(GaspowerconnectionTEST, dsmoothing_polynomial) {
  nlohmann::json innode_json;
  innode_json["id"] = "ainnode";
  double G = 4;
  double B = 123;
  Eigen::Vector2d cond0(345, 333);
  Eigen::Vector2d cond1(341, 3331);
  auto extpowerplant_json
      = externalpowerplant_json("bextpowerplant", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 55;

  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = get_Networkproblem(netprob_json);

  auto edges = netprob->get_network().get_edges();
  if (edges.size() != 1) {
    FAIL();
  }
  auto gp = dynamic_cast<
      Model::Networkproblem::Gaspowerconnection::Gaspowerconnection const *>(
      edges.front());
  if (not gp) {
    FAIL();
  }

  EXPECT_DOUBLE_EQ(
      gp->dsmoothing_polynomial_dq(gp->kappa), gas2power_q_coefficient);
  EXPECT_DOUBLE_EQ(
      gp->dsmoothing_polynomial_dq(-gp->kappa), power2gas_q_coefficient);

  EXPECT_THROW(gp->dsmoothing_polynomial_dq(gp->kappa + 1), std::runtime_error);
  EXPECT_THROW(
      gp->dsmoothing_polynomial_dq(-gp->kappa - 1), std::runtime_error);

  double h = sqrt(Aux::EPSILON);
  {
    auto numderivative = (gp->smoothing_polynomial(gp->kappa)
                          - gp->smoothing_polynomial(gp->kappa - h))
                         / h;
    EXPECT_NEAR(numderivative, gp->dsmoothing_polynomial_dq(gp->kappa), h);
  }
  {
    auto numderivative = (gp->smoothing_polynomial(-gp->kappa + h)
                          - gp->smoothing_polynomial(-gp->kappa))
                         / h;
    EXPECT_NEAR(numderivative, gp->dsmoothing_polynomial_dq(-gp->kappa), h);
  }
}

TEST_F(GaspowerconnectionTEST, generated_power) {
  nlohmann::json innode_json;
  innode_json["id"] = "ainnode";
  double G = 4;
  double B = 123;
  Eigen::Vector2d cond0(345, 333);
  Eigen::Vector2d cond1(341, 3331);
  auto extpowerplant_json
      = externalpowerplant_json("bextpowerplant", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 55;

  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = get_Networkproblem(netprob_json);

  auto edges = netprob->get_network().get_edges();
  if (edges.size() != 1) {
    FAIL();
  }
  auto gp = dynamic_cast<
      Model::Networkproblem::Gaspowerconnection::Gaspowerconnection const *>(
      edges.front());
  if (not gp) {
    FAIL();
  }

  EXPECT_DOUBLE_EQ(gp->generated_power(0), 0);

  EXPECT_DOUBLE_EQ(
      gp->generated_power(gp->kappa + 1),
      gas2power_q_coefficient * (gp->kappa + 1));

  EXPECT_DOUBLE_EQ(
      gp->generated_power(gp->kappa), gas2power_q_coefficient * gp->kappa);
  EXPECT_DOUBLE_EQ(
      gp->generated_power(-gp->kappa), power2gas_q_coefficient * (-gp->kappa));

  EXPECT_DOUBLE_EQ(
      gp->generated_power(-gp->kappa - 1),
      power2gas_q_coefficient * (-gp->kappa - 1));
}

TEST_F(GaspowerconnectionTEST, dgenerated_power_dq) {
  nlohmann::json innode_json;
  innode_json["id"] = "ainnode";
  double G = 4;
  double B = 123;
  Eigen::Vector2d cond0(345, 333);
  Eigen::Vector2d cond1(341, 3331);
  auto extpowerplant_json
      = externalpowerplant_json("bextpowerplant", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 55;

  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = get_Networkproblem(netprob_json);

  auto edges = netprob->get_network().get_edges();
  if (edges.size() != 1) {
    FAIL();
  }
  auto gp = dynamic_cast<
      Model::Networkproblem::Gaspowerconnection::Gaspowerconnection const *>(
      edges.front());
  if (not gp) {
    FAIL();
  }

  EXPECT_DOUBLE_EQ(gp->dgenerated_power_dq(0), gp->dsmoothing_polynomial_dq(0));

  EXPECT_DOUBLE_EQ(
      gp->dgenerated_power_dq(gp->kappa + 1), gas2power_q_coefficient);

  EXPECT_DOUBLE_EQ(gp->dgenerated_power_dq(gp->kappa), gas2power_q_coefficient);
  EXPECT_DOUBLE_EQ(
      gp->dgenerated_power_dq(-gp->kappa), power2gas_q_coefficient);

  EXPECT_DOUBLE_EQ(
      gp->dgenerated_power_dq(-gp->kappa - 1), power2gas_q_coefficient);
}

TEST_F(GaspowerconnectionTEST, evaluate) {

  nlohmann::json innode_json;
  innode_json["id"] = "ainnode";
  double G = 10;
  double B = 20;
  Eigen::Vector2d cond0(100, 200);
  Eigen::Vector2d cond1(100, 200);
  auto extpowerplant_json
      = externalpowerplant_json("bextpowerplant", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 554;

  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = get_Networkproblem(netprob_json);
  auto number_of_variables = netprob->set_indices(0);

  double last_time = 0.0;
  double new_time = 1.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);
  last_state.setOnes();

  for (int i = 0; i != number_of_variables; ++i) { new_state[i] = i; }
  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);
  auto edges = netprob->get_network().get_edges();
  if (edges.size() != 1) {
    FAIL();
  }

  auto nodes = netprob->get_network().get_nodes();
  if (nodes.size() != 2) {
    FAIL();
  }

  auto gp = dynamic_cast<
      Model::Networkproblem::Gaspowerconnection::Gaspowerconnection const *>(
      edges.front());
  if (not gp) {
    FAIL();
  }

  auto endpowernode
      = dynamic_cast<Model::Networkproblem::Power::Powernode const *>(
          gp->get_ending_node());
  if (not endpowernode) {
    FAIL();
  }

  for (int i = 0; i != 10; ++i) {
    new_state.setRandom();
    netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);
    EXPECT_DOUBLE_EQ(
        rootvalues[3],
        endpowernode->P(new_state) - gp->generated_power(new_state[3]));
  }
}

TEST_F(GaspowerconnectionTEST, evaluate_state_derivative) {

  nlohmann::json innode_json;
  innode_json["id"] = "ainnode";
  double G = 10;
  double B = 20;
  Eigen::Vector2d cond0(100, 200);
  Eigen::Vector2d cond1(100, 200);
  auto extpowerplant_json
      = externalpowerplant_json("bextpowerplant", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 554;

  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = get_Networkproblem(netprob_json);
  auto number_of_variables = netprob->set_indices(0);

  double last_time = 0.0;
  double new_time = 1.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);
  last_state.setOnes();

  for (int i = 0; i != number_of_variables; ++i) { new_state[i] = i; }
  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);
  auto edges = netprob->get_network().get_edges();
  if (edges.size() != 1) {
    FAIL();
  }

  auto nodes = netprob->get_network().get_nodes();
  if (nodes.size() != 2) {
    FAIL();
  }

  auto gp = dynamic_cast<
      Model::Networkproblem::Gaspowerconnection::Gaspowerconnection const *>(
      edges.front());
  if (not gp) {
    FAIL();
  }

  for (int i = 0; i != 10; ++i) {
    Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
    Aux::Triplethandler handler(&J);

    Eigen::Matrix4d DenseJ;
    new_state.setRandom();
    netprob->evaluate_state_derivative(
        &handler, last_time, new_time, last_state, new_state);
    handler.set_matrix();
    DenseJ = J;
    EXPECT_DOUBLE_EQ(DenseJ(3, 0), 2 * G * new_state[0]);
    EXPECT_DOUBLE_EQ(DenseJ(3, 1), 0);
    EXPECT_DOUBLE_EQ(DenseJ(3, 2), 0);
    EXPECT_DOUBLE_EQ(DenseJ(3, 3), -gp->dgenerated_power_dq(new_state[3]));
  }
}
