#include "Matrixhandler.hpp"
#include "PQnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::ordered_json;

struct PowerTest : public ::testing ::Test {
  virtual void SetUp() override {}
  virtual void TearDown() override {}
};

TEST(testPower, test_P_and_Q_1) {

  double V1_bd = 8.0;
  double phi1_bd = 6.0;
  double G1 = 3.0;
  double B1 = -5.0;

  double P2_bd = 3.0;
  double Q2_bd = 7.0;
  double G2 = 2.0;
  double B2 = 2.0;

  double Gt = 2.0;
  double Bt = 4.0;

  double V1 = 1.0;
  double phi1 = 2.0;
  double V2 = 3.0;
  double phi2 = 4.0;

  json bd_json1 = {
      {"id", "N1"},
      {"type", "Vphi"},
      {"data", json::array({{{"time", 0.},
                             {"values", json::array({V1_bd, phi1_bd})}}})}};
  json bd_json2 = {
      {"id", "N2"},
      {"type", "PQ"},
      {"data",
       json::array({{{"time", 0.}, {"values", json::array({P2_bd, Q2_bd})}}})}};

  Model::Networkproblem::Power::Vphinode n1("N1", bd_json1, G1, B1);
  Model::Networkproblem::Power::PQnode n2("N2", bd_json2, G2, B2);
  Model::Networkproblem::Power::Transmissionline t("T1", &n1, &n2, Gt, Bt);

  auto a = n1.set_indices(0);
  n2.set_indices(a);

  Eigen::VectorXd rootfunction(4);
  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd last_state(4);
  Eigen::VectorXd new_state(4);
  new_state << V1, phi1, V2, phi2;

  n1.evaluate(rootfunction, last_time, new_time, last_state, new_state);
  n2.evaluate(rootfunction, last_time, new_time, last_state, new_state);

  EXPECT_DOUBLE_EQ(rootfunction[0], new_state[0] - V1_bd);
  EXPECT_DOUBLE_EQ(rootfunction[1], new_state[1] - phi1_bd);
  EXPECT_DOUBLE_EQ(rootfunction[2],
                   -P2_bd + G2 * V2 * V2 +
                       V2 * V1 *
                           (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(rootfunction[3],
                   -Q2_bd - B2 * V2 * V2 +
                       V2 * V1 *
                           (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  n1.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  n2.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  handler.set_matrix();

  EXPECT_EQ(J.nonZeros(), 10);
  EXPECT_DOUBLE_EQ(J.coeff(0, 0), 1.0);
  EXPECT_DOUBLE_EQ(J.coeff(1, 1), 1.0);

  EXPECT_DOUBLE_EQ(J.coeff(2, 0),
                   V2 * (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(J.coeff(2, 1),
                   V1 * V2 * (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(2, 2), 2 * G2 * V2 + V1 * (Gt * cos(phi2 - phi1) +
                                                      Bt * sin(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(2, 3),
                   V2 * V1 * (-Gt * sin(phi2 - phi1) + Bt * cos(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(3, 0),
                   V2 * (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(3, 1),
                   V2 * V1 * (-Gt * cos(phi2 - phi1) - Bt * sin(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(3, 2), -2 * B2 * V2 + V1 * (Gt * sin(phi2 - phi1) -
                                                       Bt * cos(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(J.coeff(3, 3),
                   V2 * V1 * (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
};

TEST(testPower, test_P_and_Q_2) {

  double V1_bd = 8.0;
  double phi1_bd = 6.0;
  double G1 = 3.0;
  double B1 = -5.0;

  double P2_bd = 3.0;
  double Q2_bd = 7.0;
  double G2 = 2.0;
  double B2 = 2.0;

  double Gt = 2.0;
  double Bt = 4.0;

  double V1 = 1.0;
  double phi1 = 2.0;
  double V2 = 3.0;
  double phi2 = 4.0;
  // double V1_bd = 8.0;
  // double phi1_bd = 1.0;
  // double G1 = 1.0;
  // double B1 = 1.0;

  // double P2_bd = 3.0;
  // double Q2_bd = 7.0;
  // double G2 = 2.0;
  // double B2 = 2.0;

  // double Gt = 2.0;
  // double Bt = 4.0;

  // double V1 = 1.0;
  // double phi1 = 1.0;
  // double V2 = 1.0;
  // double phi2 = 1.0;

  json bd_json1 = {
      {"id", "N1"},
      {"type", "Vphi"},
      {"data", json::array({{{"time", 0.},
                             {"values", json::array({V1_bd, phi1_bd})}}})}};
  json bd_json2 = {
      {"id", "N2"},
      {"type", "PQ"},
      {"data",
       json::array({{{"time", 0.}, {"values", json::array({P2_bd, Q2_bd})}}})}};

  Model::Networkproblem::Power::Vphinode n1("N1", bd_json1, G1, B1);
  Model::Networkproblem::Power::PQnode n2("N2", bd_json2, G2, B2);
  Model::Networkproblem::Power::Transmissionline t("T1", &n2, &n1, Gt, Bt);

  auto a = n1.set_indices(0);
  n2.set_indices(a);

  Eigen::VectorXd rootfunction(4);
  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd last_state(4);
  Eigen::VectorXd new_state(4);
  new_state << V1, phi1, V2, phi2;

  n1.evaluate(rootfunction, last_time, new_time, last_state, new_state);
  n2.evaluate(rootfunction, last_time, new_time, last_state, new_state);

  EXPECT_DOUBLE_EQ(rootfunction[0], new_state[0] - V1_bd);
  EXPECT_DOUBLE_EQ(rootfunction[1], new_state[1] - phi1_bd);
  EXPECT_DOUBLE_EQ(rootfunction[2],
                   -P2_bd + G2 * V2 * V2 +
                       V2 * V1 *
                           (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(rootfunction[3],
                   -Q2_bd - B2 * V2 * V2 +
                       V2 * V1 *
                           (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);

  n1.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  n2.evaluate_state_derivative(&handler, last_time, new_time, last_state,
                               new_state);
  handler.set_matrix();

  EXPECT_EQ(J.nonZeros(), 10);

  EXPECT_DOUBLE_EQ(J.coeff(0, 0), 1.0);
  EXPECT_DOUBLE_EQ(J.coeff(1, 1), 1.0);

  EXPECT_DOUBLE_EQ(J.coeff(2, 0),
                   V2 * (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(J.coeff(2, 1),
                   V1 * V2 * (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(2, 2), 2 * G2 * V2 + V1 * (Gt * cos(phi2 - phi1) +
                                                      Bt * sin(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(2, 3),
                   V2 * V1 * (-Gt * sin(phi2 - phi1) + Bt * cos(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(3, 0),
                   V2 * (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(3, 1),
                   V2 * V1 * (-Gt * cos(phi2 - phi1) - Bt * sin(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(3, 2), -2 * B2 * V2 + V1 * (Gt * sin(phi2 - phi1) -
                                                       Bt * cos(phi2 - phi1)));
  EXPECT_DOUBLE_EQ(J.coeff(3, 3),
                   V2 * V1 * (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));

  EXPECT_DOUBLE_EQ(J.coeff(0, 0), 1.0);
  EXPECT_DOUBLE_EQ(J.coeff(1, 1), 1.0);

  //   EXPECT_DOUBLE_EQ(J.coeff(2, 0),
  //                    V2 * (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
  //   EXPECT_DOUBLE_EQ(J.coeff(2, 1),
  //                    V1 * V2 * (Gt * sin(phi2 - phi1) - Bt * cos(phi2 -
  //                    phi1)));

  //   EXPECT_DOUBLE_EQ(J.coeff(2, 2), 2 * G2 * V2 + V1 * (Gt * cos(phi2 - phi1)
  //   +
  //                                                       Bt * sin(phi2 -
  //                                                       phi1)));

  //   EXPECT_DOUBLE_EQ(J.coeff(2, 3),
  //                    V2 * V1 * (-Gt * sin(phi2 - phi1) + Bt * cos(phi2 -
  //                    phi1)));

  //   EXPECT_DOUBLE_EQ(J.coeff(3, 0),
  //                    V1 * (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

  //   EXPECT_DOUBLE_EQ(J.coeff(3, 1),
  //                    V2 * V1 * (-Gt * cos(phi2 - phi1) - Bt * sin(phi2 -
  //                    phi1)));

  //   EXPECT_DOUBLE_EQ(J.coeff(3, 2), -2 * B2 * V2 + V1 * (Gt * sin(phi2 -
  //   phi1) -
  //                                                        Bt * cos(phi2 -
  //                                                        phi1)));
  //   EXPECT_DOUBLE_EQ(J.coeff(3, 3),
  //                    V2 * V1 * (Gt * cos(phi2 - phi1) + Bt * sin(phi2 -
  //                    phi1)));
};
