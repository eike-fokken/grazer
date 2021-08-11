#include "StochasticPQnode.hpp"
#include "Equationcomponent_test_helpers.hpp"
#include "powertest_helpers.hpp"

#include "Matrixhandler.hpp"
#include "Net.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Power_factory.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

#include <Eigen/Sparse>
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

nlohmann::json stochpq_json(
    std::string id, double G, double B, Eigen::Vector2d bd0,
    Eigen::Vector2d bd1, int number_of_stochastic_steps, double theta_P,
    double sigma_P, double theta_Q, double sigma_Q);

class stochasticPQnodeTEST : public EqcomponentTEST {
  Model::Componentfactory::Power_factory factory{R"({})"_json};

public:
  // values for the Vphinode:
  std::string id1 = "vphi";
  double V1_bd = 8.0;
  double phi1_bd = 6.0;
  double G1 = 3.0;
  double B1 = -5.0;

  // values for the StochasticPQnode:
  std::string id2 = "stochpq";
  double P2_bd = 3.0;
  double Q2_bd = 7.0;
  double G2 = 2.0;
  double B2 = 2.0;
  int number_of_stochastic_steps{1000};
  double theta_P = 2.0;
  double sigma_P = 0.5;
  double theta_Q = 3.0;
  double sigma_Q = 0.3;

  // values for the PVnode:
  std::string id3 = "pv";
  double P3_bd = 4.0;
  double V3_bd = 19.0;
  double G3 = 9.0;
  double B3 = 2.0;

  // Transmissionline from Vphi to PQ:
  std::string idt1 = "tl1";
  double Gt1 = 2.0;
  double Bt1 = 4.0;

  // Transmissionline from PQ to PV:
  std::string idt2 = "tl2";
  double Gt2 = 20.0;
  double Bt2 = 79.0;

  // initial values:
  double V1 = 1.0;
  double phi1 = 2.0;
  double V2 = 3.0;
  double phi2 = 4.0;
  double V3 = 5.0;
  double phi3 = 89.0;

  std::unique_ptr<Model::Networkproblem::Networkproblem>
  make_Networkproblem(nlohmann::json &netproblem);

  std::tuple<
      std::unique_ptr<Model::Networkproblem::Networkproblem>, double, double,
      Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd>
  default_setup();
};

TEST_F(stochasticPQnodeTEST, evaluate) {

  auto [netprob, last_time, new_time, last_state, new_state, rootvalues]
      = default_setup();
  netprob->prepare_timestep(last_time, new_time, last_state, new_state);
  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);

  auto *stoch_pq
      = dynamic_cast<Model::Networkproblem::Power::StochasticPQnode *>(
          netprob->get_network().get_node_by_id("stochpq"));
  if (not stoch_pq) {
    FAIL();
  }

  auto *vphi = dynamic_cast<Model::Networkproblem::Power::Powernode *>(
      netprob->get_network().get_node_by_id("vphi"));
  if (not vphi) {
    FAIL();
  }
  auto *pv = dynamic_cast<Model::Networkproblem::Power::Powernode *>(
      netprob->get_network().get_node_by_id("pv"));
  if (not pv) {
    FAIL();
  }

  // Here we seem to run into floating point problems, maybe because of the many
  // digits of the random numbers. Expecting only to be equal up to 1e-12 seems
  // to work well and is hopefully sufficient to catch regressions.

  EXPECT_NEAR(
      rootvalues[stoch_pq->get_start_state_index()],
      -stoch_pq->get_current_P() + G2 * V2 * V2
          + V2 * V1 * (Gt1 * cos(phi2 - phi1) + Bt1 * sin(phi2 - phi1))
          + V2 * V3 * (Gt2 * cos(phi2 - phi3) + Bt2 * sin(phi2 - phi3)),
      1e-12);

  EXPECT_NEAR(
      rootvalues[stoch_pq->get_start_state_index() + 1],
      -stoch_pq->get_current_Q() - B2 * V2 * V2
          + V2 * V1 * (Gt1 * sin(phi2 - phi1) - Bt1 * cos(phi2 - phi1))
          + V2 * V3 * (Gt2 * sin(phi2 - phi3) - Bt2 * cos(phi2 - phi3)),
      1e-12);
}

TEST_F(stochasticPQnodeTEST, evaluate_state_derivative) {

  auto [netprob, last_time, new_time, last_state, new_state, rootvalues]
      = default_setup();

  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);

  auto *vphi = dynamic_cast<Model::Networkproblem::Power::Powernode *>(
      netprob->get_network().get_node_by_id("vphi"));
  if (not vphi) {
    FAIL();
  }
  auto *pq = dynamic_cast<Model::Networkproblem::Power::Powernode *>(
      netprob->get_network().get_node_by_id("stochpq"));
  if (not pq) {
    FAIL();
  }
  auto *pv = dynamic_cast<Model::Networkproblem::Power::Powernode *>(
      netprob->get_network().get_node_by_id("pv"));

  if (not pv) {
    FAIL();
  }

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);
  netprob->evaluate_state_derivative(
      &handler, last_time, new_time, last_state, new_state);
  handler.set_matrix();

  Eigen::Matrix<double, 6, 6> DenseJ = J;
  auto index0_vphi = vphi->get_start_state_index();
  auto index1_vphi = vphi->get_start_state_index() + 1;
  auto index0_pq = pq->get_start_state_index();
  auto index1_pq = pq->get_start_state_index() + 1;
  auto index0_pv = pv->get_start_state_index();
  auto index1_pv = pv->get_start_state_index() + 1;

  // derivatives of P in the PQ node:
  EXPECT_DOUBLE_EQ(
      DenseJ(index0_pq, index0_vphi),
      V2 * (Gt1 * cos(phi2 - phi1) + Bt1 * sin(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index0_pq, index1_vphi),
      V1 * V2 * (Gt1 * sin(phi2 - phi1) - Bt1 * cos(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index0_pq, index0_pq),
      2 * G2 * V2 + V1 * (Gt1 * cos(phi2 - phi1) + Bt1 * sin(phi2 - phi1))
          + V3 * (Gt2 * cos(phi2 - phi3) + Bt2 * sin(phi2 - phi3)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index0_pq, index1_pq),
      V2 * V1 * (-Gt1 * sin(phi2 - phi1) + Bt1 * cos(phi2 - phi1))
          + V2 * V3 * (-Gt2 * sin(phi2 - phi3) + Bt2 * cos(phi2 - phi3)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index0_pq, index0_pv),
      V2 * (Gt2 * cos(phi2 - phi3) + Bt2 * sin(phi2 - phi3)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index0_pq, index1_pv),
      V3 * V2 * (Gt2 * sin(phi2 - phi3) - Bt2 * cos(phi2 - phi3)));

  // derivatives of index 3, that is, Q in the PQ node:
  EXPECT_DOUBLE_EQ(
      DenseJ(index1_pq, index0_vphi),
      V2 * (Gt1 * sin(phi2 - phi1) - Bt1 * cos(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index1_pq, index1_vphi),
      V2 * V1 * (-Gt1 * cos(phi2 - phi1) - Bt1 * sin(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index1_pq, index0_pq),
      -2 * B2 * V2 + V1 * (Gt1 * sin(phi2 - phi1) - Bt1 * cos(phi2 - phi1))
          + V3 * (Gt2 * sin(phi2 - phi3) - Bt2 * cos(phi2 - phi3)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index1_pq, index1_pq),
      V2 * V1 * (Gt1 * cos(phi2 - phi1) + Bt1 * sin(phi2 - phi1))
          + V2 * V3 * (Gt2 * cos(phi2 - phi3) + Bt2 * sin(phi2 - phi3)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index1_pq, index0_pv),
      V2 * (Gt2 * sin(phi2 - phi3) - Bt2 * cos(phi2 - phi3)));
  EXPECT_DOUBLE_EQ(
      DenseJ(index1_pq, index1_pv),
      V2 * V3 * (-Gt2 * cos(phi2 - phi3) - Bt2 * sin(phi2 - phi3)));
}

/**/ //////////////////////////////////////////////////////
/**/ // Here come the definitions of the fixture methods:
/**/ /////////////////////////////////////////////////////

nlohmann::json stochpq_json(
    std::string id, double G, double B, Eigen::Vector2d bd0,
    Eigen::Vector2d bd1, int number_of_stochastic_steps, double theta_P,
    double sigma_P, double theta_Q, double sigma_Q) {

  nlohmann::json stochpq_json;
  stochpq_json["id"] = id;
  stochpq_json["G"] = G;
  stochpq_json["B"] = B;
  stochpq_json["number_of_stochastic_steps"] = number_of_stochastic_steps;
  stochpq_json["theta_P"] = theta_P;
  stochpq_json["sigma_P"] = sigma_P;
  stochpq_json["theta_Q"] = theta_Q;
  stochpq_json["sigma_Q"] = sigma_Q;

  auto b0 = make_value_json(id, "time", bd0, bd1);
  stochpq_json["boundary_values"] = b0;
  return stochpq_json;
}

std::unique_ptr<Model::Networkproblem::Networkproblem>
stochasticPQnodeTEST::make_Networkproblem(nlohmann::json &netproblem) {
  return EqcomponentTEST::make_Networkproblem(netproblem, factory);
}

std::tuple<
    std::unique_ptr<Model::Networkproblem::Networkproblem>, double, double,
    Eigen::VectorXd, Eigen::VectorXd, Eigen::VectorXd>
stochasticPQnodeTEST::default_setup() {

  Eigen::Vector2d bdcond1{V1_bd, phi1_bd};
  auto vphi_json = powernode_json(id1, G1, B1, bdcond1, bdcond1);

  Eigen::Vector2d bdcond2{P2_bd, Q2_bd};
  auto stochasticpq_json = stochpq_json(
      id2, G2, B2, bdcond2, bdcond2, number_of_stochastic_steps, theta_P,
      sigma_P, theta_Q, sigma_Q);

  Eigen::Vector2d bdcond3{P3_bd, V3_bd};
  auto pv_json = powernode_json(id3, G3, B3, bdcond3, bdcond3);

  auto tl1_json
      = transmissionline_json(idt1, Gt1, Bt1, vphi_json, stochasticpq_json);
  auto tl2_json
      = transmissionline_json(idt2, Gt2, Bt2, stochasticpq_json, pv_json);

  auto np_json = make_full_json(
      {{"Vphinode", {vphi_json}},
       {"StochasticPQnode", {stochasticpq_json}},
       {"PVnode", {pv_json}}},
      {{"Transmissionline", {tl1_json, tl2_json}}});

  auto netprob = make_Networkproblem(np_json);
  int number_of_variables = netprob->set_indices(0);

  Eigen::Vector4d cond1(
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(), V1, phi1);
  Eigen::Vector4d cond2(
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(), V2, phi2);
  Eigen::Vector4d cond3(
      std::numeric_limits<double>::quiet_NaN(),
      std::numeric_limits<double>::quiet_NaN(), V3, phi3);

  auto init1 = std::vector{std::make_pair(0.0, cond1)};
  auto initial_json1 = make_value_json(id1, "x", init1);
  auto init2 = std::vector{std::make_pair(0.0, cond2)};
  auto initial_json2 = make_value_json(id2, "x", init2);
  auto init3 = std::vector{std::make_pair(0.0, cond3)};
  auto initial_json3 = make_value_json(id3, "x", init3);

  nlohmann::json np_initialjson = make_initial_json(
      {}, {{"Vphinode", {initial_json1}},
           {"StochasticPQnode", {initial_json2}},
           {"PVnode", {initial_json3}}});

  double last_time = 0.0;
  double new_time = 1.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  netprob->set_initial_values(last_state, np_initialjson);
  Eigen::VectorXd new_state = last_state;
  return std::make_tuple(
      std::move(netprob), last_time, new_time, last_state, new_state,
      rootvalues);
}
