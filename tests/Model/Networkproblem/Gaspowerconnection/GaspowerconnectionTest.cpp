#include "Gaspowerconnection.hpp"
#include "Equationcomponent.hpp"
#include "Equationcomponent_test_helpers.hpp"
#include "ExternalPowerplant.hpp"
#include "Full_factory.hpp"
#include "Innode.hpp"
#include "Mathfunctions.hpp"
#include "Matrixhandler.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "Powernode.hpp"
#include <Eigen/src/Core/Matrix.h>
#include <filesystem>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <stdexcept>

class GaspowerconnectionTEST : public EqcomponentTEST {
public:
  Model::Componentfactory::Full_factory factory{R"({})"_json};

public:
  std::unique_ptr<Model::Networkproblem::Networkproblem>
  make_Networkproblem(nlohmann::json &netproblem) {
    return EqcomponentTEST::make_Networkproblem(netproblem, factory);
  }
};

nlohmann::json gaspowerconnection_json(
    std::string id, nlohmann::json startnode, nlohmann::json endnode,
    double gas2power_q_coefficient, double power2gas_q_coefficient,
    double bdcond0, double bdcond1, double control0, double control1);
nlohmann::json externalpowerplant_json(
    std::string id, double G, double B, Eigen::Vector2d cond0,
    Eigen::Vector2d cond1);

TEST_F(GaspowerconnectionTEST, smoothing_polynomial) {
  nlohmann::json innode_json;
  innode_json["id"] = "ainnode";
  double G = 4;
  double B = 123;
  Eigen::Vector2d cond0(345, 333);
  Eigen::Vector2d cond1(341, 3331);
  auto extpowerplant_json
      = externalpowerplant_json("extpowerplant", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 55;

  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient, 1, 1, 0, 0);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = make_Networkproblem(netprob_json);

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
  try {
    gp->smoothing_polynomial(gp->kappa + 1);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("You can't call this function for values "
                                     "of q bigger than"));
  }
  try {
    gp->smoothing_polynomial(-gp->kappa - 1);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("You can't call this function for values "
                                     "of q bigger than"));
  }
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
      power2gas_q_coefficient, 1, 1, 0, 0);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = make_Networkproblem(netprob_json);

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

  try {
    gp->dsmoothing_polynomial_dq(gp->kappa + 1);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("You can't call this function for values "
                                     "of q bigger than"));
  }
  try {
    gp->dsmoothing_polynomial_dq(-gp->kappa - 1);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("You can't call this function for values "
                                     "of q bigger than"));
  }

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
      power2gas_q_coefficient, 1, 1, 0, 0);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = make_Networkproblem(netprob_json);

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
      power2gas_q_coefficient, 1, 1, 0, 0);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = make_Networkproblem(netprob_json);

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

TEST_F(GaspowerconnectionTEST, evaluate_power_controlled) {

  nlohmann::json innode_json;
  innode_json["id"] = "ainnode";
  double G = 10;
  double B = 20;
  Eigen::Vector2d cond0(100, 200);
  Eigen::Vector2d cond1(150, 250);
  auto extpowerplant_json
      = externalpowerplant_json("bextpowerplant", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 554;

  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient, 1, 1, 0, 0);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = make_Networkproblem(netprob_json);
  auto number_of_variables = netprob->set_state_indices(0);

  double last_time = 0.0;
  double new_time = 1.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);
  last_state.setOnes();

  for (int i = 0; i != number_of_variables; ++i) { new_state[i] = i; }
  // The following are not needed, as the components used here up to now are not
  // controlled.  But to satisfy the interface, we must provide them.
  Eigen::VectorXd last_control;
  Eigen::VectorXd control;

  netprob->evaluate(
      rootvalues, last_time, new_time, last_state, new_state, 
      control);
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
    netprob->evaluate(
        rootvalues, last_time, new_time, last_state, new_state, control);
    EXPECT_DOUBLE_EQ(
        rootvalues[gp->get_start_state_index() + 1],
        endpowernode->P(new_state)
            - gp->generated_power(new_state[gp->get_start_state_index() + 1]));

    // The following checks that the equations of the powernode are indeed
    // those of a Vphi-node. This must be true in Power-controlled mode of the
    // Gaspowerconnection.
    EXPECT_DOUBLE_EQ(
        rootvalues[endpowernode->get_start_state_index()],
        new_state[endpowernode->get_start_state_index()] - cond1(0));
    EXPECT_DOUBLE_EQ(
        rootvalues[endpowernode->get_start_state_index() + 1],
        new_state[endpowernode->get_start_state_index() + 1] - cond1(1));
  }
}

TEST_F(GaspowerconnectionTEST, evaluate_gas_controlled) {

  nlohmann::json innode_json;
  innode_json["id"] = "ainnode";
  double G = 10;
  double B = 20;
  Eigen::Vector2d cond0(100, 200);
  Eigen::Vector2d cond1(150, 250);
  auto extpowerplant_json
      = externalpowerplant_json("bextpowerplant", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 554;
  double pressure0 = 144.;
  double pressure1 = 19.;
  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient, pressure0, pressure1, 1, 1);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = make_Networkproblem(netprob_json);
  auto number_of_variables = netprob->set_state_indices(0);

  double last_time = 0.0;
  double new_time = 1.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);
  last_state.setOnes();

  for (int i = 0; i != number_of_variables; ++i) { new_state[i] = i; }
  // The following are not needed, as the components used here up to now are not
  // controlled.  But to satisfy the interface, we must provide them.
  Eigen::VectorXd last_control;
  Eigen::VectorXd control;
  netprob->evaluate(
      rootvalues, last_time, new_time, last_state, new_state, 
      control);
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
    netprob->evaluate(
        rootvalues, last_time, new_time, last_state, new_state, control);
    EXPECT_DOUBLE_EQ(
        rootvalues[gp->get_start_state_index() + 1],
        endpowernode->P(new_state)
            - gp->generated_power(new_state[gp->get_start_state_index() + 1]));

    // The following makes sure that the the equations of the endpowernode set
    // the pressure to the connection boundary condition and the voltage to the
    // voltage boundary condition.
    EXPECT_DOUBLE_EQ(
        rootvalues[endpowernode->get_start_state_index()],
        new_state[gp->get_start_state_index()] - pressure1);
    EXPECT_DOUBLE_EQ(
        rootvalues[endpowernode->get_start_state_index() + 1],
        new_state[endpowernode->get_start_state_index()] - cond1(0));
  }
}

TEST_F(GaspowerconnectionTEST, d_evalutate_d_new_state_power_driven) {

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
      power2gas_q_coefficient, 1, 1, 0, 0);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = make_Networkproblem(netprob_json);
  auto number_of_variables = netprob->set_state_indices(0);

  double last_time = 0.0;
  double new_time = 1.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);
  last_state.setOnes();

  for (int i = 0; i != number_of_variables; ++i) { new_state[i] = i; }
  // The following are not needed, as the components used here up to now are not
  // controlled.  But to satisfy the interface, we must provide them.
  Eigen::VectorXd last_control;
  Eigen::VectorXd control;
  netprob->evaluate(
      rootvalues, last_time, new_time, last_state, new_state, 
      control);
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
    Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
    Aux::Triplethandler handler(J);

    Eigen::Matrix4d DenseJ;
    new_state.setRandom();
    netprob->d_evalutate_d_new_state(
        handler, last_time, new_time, last_state, new_state, control);
    handler.set_matrix();
    DenseJ = J;
    EXPECT_DOUBLE_EQ(
        DenseJ(
            gp->get_start_state_index() + 1,
            endpowernode->get_start_state_index()),
        2 * G * new_state[0]);
    EXPECT_DOUBLE_EQ(
        DenseJ(
            gp->get_start_state_index() + 1,
            endpowernode->get_start_state_index() + 1),
        0);
    EXPECT_DOUBLE_EQ(
        DenseJ(gp->get_start_state_index() + 1, gp->get_start_state_index()),
        0);
    EXPECT_DOUBLE_EQ(
        DenseJ(
            gp->get_start_state_index() + 1, gp->get_start_state_index() + 1),
        -gp->dgenerated_power_dq(new_state[gp->get_start_state_index() + 1]));

    EXPECT_DOUBLE_EQ(
        DenseJ(
            endpowernode->get_start_state_index(),
            endpowernode->get_start_state_index()),
        1.0);
    for (int index = 0; index != 4; ++index) {
      if (index == endpowernode->get_start_state_index()) {
        continue;
      }
      EXPECT_DOUBLE_EQ(
          DenseJ(endpowernode->get_start_state_index(), index), 0.0);
    }
  }
}

TEST_F(GaspowerconnectionTEST, d_evalutate_d_new_state_gas_driven) {

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
  double pressure0 = 144.;
  double pressure1 = 19.;
  auto gp_json = gaspowerconnection_json(
      "gp", innode_json, extpowerplant_json, gas2power_q_coefficient,
      power2gas_q_coefficient, pressure0, pressure1, 1, 1);

  auto netprob_json = make_full_json(
      {{"Innode", {innode_json}}, {"ExternalPowerplant", {extpowerplant_json}}},
      {{"Gaspowerconnection", {gp_json}}});
  auto netprob = make_Networkproblem(netprob_json);
  auto number_of_variables = netprob->set_state_indices(0);

  double last_time = 0.0;
  double new_time = 1.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);
  last_state.setOnes();

  for (int i = 0; i != number_of_variables; ++i) { new_state[i] = i; }
  // The following are not needed, as the components used here up to now are not
  // controlled.  But to satisfy the interface, we must provide them.
  Eigen::VectorXd last_control;
  Eigen::VectorXd control;
  netprob->evaluate(
      rootvalues, last_time, new_time, last_state, new_state, 
      control);
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
    Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
    Aux::Triplethandler handler(J);

    Eigen::Matrix4d DenseJ;
    new_state.setRandom();
    netprob->d_evalutate_d_new_state(
        handler, last_time, new_time, last_state, new_state, control);
    handler.set_matrix();
    DenseJ = J;
    EXPECT_DOUBLE_EQ(
        DenseJ(
            gp->get_start_state_index() + 1,
            endpowernode->get_start_state_index()),
        2 * G * new_state[0]);
    EXPECT_DOUBLE_EQ(
        DenseJ(
            gp->get_start_state_index() + 1,
            endpowernode->get_start_state_index() + 1),
        0);
    EXPECT_DOUBLE_EQ(
        DenseJ(gp->get_start_state_index() + 1, gp->get_start_state_index()),
        0);
    EXPECT_DOUBLE_EQ(
        DenseJ(
            gp->get_start_state_index() + 1, gp->get_start_state_index() + 1),
        -gp->dgenerated_power_dq(new_state[gp->get_start_state_index() + 1]));

    EXPECT_DOUBLE_EQ(
        DenseJ(
            endpowernode->get_start_state_index(), gp->get_start_state_index()),
        1.0);
    for (int index = 0; index != 4; ++index) {
      if (index == gp->get_start_state_index()) {
        continue;
      }
      EXPECT_DOUBLE_EQ(
          DenseJ(endpowernode->get_start_state_index(), index), 0.0);
    }
  }
}

nlohmann::json gaspowerconnection_json(
    std::string id, nlohmann::json startnode, nlohmann::json endnode,
    double gas2power_q_coefficient, double power2gas_q_coefficient,
    double bdcond0, double bdcond1, double control0, double control1) {
  nlohmann::json gp_topology;
  gp_topology["id"] = id;
  gp_topology["from"] = startnode["id"];
  gp_topology["to"] = endnode["id"];
  gp_topology["gas2power_q_coeff"] = gas2power_q_coefficient;
  gp_topology["power2gas_q_coeff"] = power2gas_q_coefficient;
  Eigen::Matrix<double, 1, 1> cond0;
  cond0 << bdcond0;
  Eigen::Matrix<double, 1, 1> cond1;
  cond1 << bdcond1;
  auto bound = make_value_json(id, "time", cond0, cond1);
  gp_topology["boundary_values"] = bound;
  Eigen::Matrix<double, 1, 1> control_vector0;
  control_vector0 << control0;
  Eigen::Matrix<double, 1, 1> control_vector1;
  control_vector1 << control1;
  auto control = make_value_json(id, "time", control_vector0, control_vector1);
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
