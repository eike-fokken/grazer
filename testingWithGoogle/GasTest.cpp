#include <Flowboundarynode.hpp>
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
  json sp_initial = {
                     {"id", "node_4_ld1"},
                     {"data", json::array({{{"x", 0.0},
                                            {"value", json::array({pressure_start,flow_start})}},{{"x", 1.0},{"value", json::array({pressure_end,flow_end})}}})}};
  //std::cout << sp_initial<<std::endl;


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

  sp0.set_initial_values(new_state,sp_initial);
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

  EXPECT_EQ(J.nonZeros(),6);

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

// TEST(testPower, test_P_and_Q_2) {

//   double V1_bd = 8.0;
//   double phi1_bd = 6.0;
//   double G1 = 3.0;
//   double B1 = -5.0;

//   double P2_bd = 3.0;
//   double Q2_bd = 7.0;
//   double G2 = 2.0;
//   double B2 = 2.0;

//   double Gt = 2.0;
//   double Bt = 4.0;

//   double V1 = 1.0;
//   double phi1 = 2.0;
//   double V2 = 3.0;
//   double phi2 = 4.0;
//   // double V1_bd = 8.0;
//   // double phi1_bd = 1.0;
//   // double G1 = 1.0;
//   // double B1 = 1.0;

//   // double P2_bd = 3.0;
//   // double Q2_bd = 7.0;
//   // double G2 = 2.0;
//   // double B2 = 2.0;

//   // double Gt = 2.0;
//   // double Bt = 4.0;

//   // double V1 = 1.0;
//   // double phi1 = 1.0;
//   // double V2 = 1.0;
//   // double phi2 = 1.0;

//   json bd_json1 = {
//       {"id", "N1"},
//       {"type", "Vphi"},
//       {"data", json::array({{{"time", 0.},
//                              {"values", json::array({V1_bd, phi1_bd})}}})}};
//   json bd_json2 = {
//       {"id", "N2"},
//       {"type", "PQ"},
//       {"data",
//        json::array({{{"time", 0.}, {"values", json::array({P2_bd, Q2_bd})}}})}};

//   Model::Networkproblem::Power::Vphinode n1("N1", bd_json1, G1, B1);
//   Model::Networkproblem::Power::PQnode n2("N2", bd_json2, G2, B2);
//   Model::Networkproblem::Power::Transmissionline t("T1", &n2, &n1, Gt, Bt);

//   auto a = n1.set_indices(0);
//   n2.set_indices(a);

//   Eigen::VectorXd rootvalues(4);
//   double last_time = 0.0;
//   double new_time = 0.0;
//   Eigen::VectorXd last_state(4);
//   Eigen::VectorXd new_state(4);
//   new_state << V1, phi1, V2, phi2;

//   n1.evaluate(rootvalues, last_time, new_time, last_state, new_state);
//   n2.evaluate(rootvalues, last_time, new_time, last_state, new_state);

//   EXPECT_DOUBLE_EQ(rootvalues[0], new_state[0] - V1_bd);
//   EXPECT_DOUBLE_EQ(rootvalues[1], new_state[1] - phi1_bd);
//   EXPECT_DOUBLE_EQ(rootvalues[2],
//                    -P2_bd + G2 * V2 * V2 +
//                        V2 * V1 *
//                            (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
//   EXPECT_DOUBLE_EQ(rootvalues[3],
//                    -Q2_bd - B2 * V2 * V2 +
//                        V2 * V1 *
//                            (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

//   Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
//   Aux::Triplethandler handler(&J);

//   n1.evaluate_state_derivative(&handler, last_time, new_time, last_state,
//                                new_state);
//   n2.evaluate_state_derivative(&handler, last_time, new_time, last_state,
//                                new_state);
//   handler.set_matrix();

//   EXPECT_EQ(J.nonZeros(), 10);

//   EXPECT_DOUBLE_EQ(J.coeff(0, 0), 1.0);
//   EXPECT_DOUBLE_EQ(J.coeff(1, 1), 1.0);

//   EXPECT_DOUBLE_EQ(J.coeff(2, 0),
//                    V2 * (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
//   EXPECT_DOUBLE_EQ(J.coeff(2, 1),
//                    V1 * V2 * (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

//   EXPECT_DOUBLE_EQ(J.coeff(2, 2), 2 * G2 * V2 + V1 * (Gt * cos(phi2 - phi1) +
//                                                       Bt * sin(phi2 - phi1)));

//   EXPECT_DOUBLE_EQ(J.coeff(2, 3),
//                    V2 * V1 * (-Gt * sin(phi2 - phi1) + Bt * cos(phi2 - phi1)));

//   EXPECT_DOUBLE_EQ(J.coeff(3, 0),
//                    V2 * (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

//   EXPECT_DOUBLE_EQ(J.coeff(3, 1),
//                    V2 * V1 * (-Gt * cos(phi2 - phi1) - Bt * sin(phi2 - phi1)));

//   EXPECT_DOUBLE_EQ(J.coeff(3, 2), -2 * B2 * V2 + V1 * (Gt * sin(phi2 - phi1) -
//                                                        Bt * cos(phi2 - phi1)));
//   EXPECT_DOUBLE_EQ(J.coeff(3, 3),
//                    V2 * V1 * (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
// }

// TEST(testPower, test_PV) {

//   double V1_bd = 8.0;
//   double phi1_bd = 6.0;
//   double G1 = 3.0;
//   double B1 = -5.0;

//   double P2_bd = 3.0;
//   double V2_bd = 7.0;
//   double G2 = 2.0;
//   double B2 = 2.0;

//   double Gt = 2.0;
//   double Bt = 4.0;

//   double V1 = 1.0;
//   double phi1 = 2.0;
//   double V2 = 3.0;
//   double phi2 = 4.0;

//   json bd_json1 = {
//       {"id", "N1"},
//       {"type", "Vphi"},
//       {"data", json::array({{{"time", 0.},
//                              {"values", json::array({V1_bd, phi1_bd})}}})}};
//   json bd_json2 = {
//       {"id", "N2"},
//       {"type", "PV"},
//       {"data",
//        json::array({{{"time", 0.}, {"values", json::array({P2_bd, V2_bd})}}})}};

//   Model::Networkproblem::Power::Vphinode n1("N1", bd_json1, G1, B1);
//   Model::Networkproblem::Power::PVnode n2("N2", bd_json2, G2, B2);
//   Model::Networkproblem::Power::Transmissionline t("T1", &n2, &n1, Gt, Bt);

//   auto a = n1.set_indices(0);
//   n2.set_indices(a);

//   Eigen::VectorXd rootvalues(4);
//   double last_time = 0.0;
//   double new_time = 0.0;
//   Eigen::VectorXd last_state(4);
//   Eigen::VectorXd new_state(4);
//   new_state << V1, phi1, V2, phi2;

//   n1.evaluate(rootvalues, last_time, new_time, last_state, new_state);
//   n2.evaluate(rootvalues, last_time, new_time, last_state, new_state);

//   EXPECT_DOUBLE_EQ(rootvalues[0], new_state[0] - V1_bd);
//   EXPECT_DOUBLE_EQ(rootvalues[1], new_state[1] - phi1_bd);
//   EXPECT_DOUBLE_EQ(rootvalues[2],
//                    -P2_bd + G2 * V2 * V2 +
//                        V2 * V1 *
//                            (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
//   EXPECT_DOUBLE_EQ(rootvalues[3], new_state[2] - V2_bd);

//   Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
//   Aux::Triplethandler handler(&J);

//   n1.evaluate_state_derivative(&handler, last_time, new_time, last_state,
//                                new_state);
//   n2.evaluate_state_derivative(&handler, last_time, new_time, last_state,
//                                new_state);
//   handler.set_matrix();

//   EXPECT_EQ(J.nonZeros(), 7);

//   EXPECT_DOUBLE_EQ(J.coeff(0, 0), 1.0);
//   EXPECT_DOUBLE_EQ(J.coeff(1, 1), 1.0);

//   EXPECT_DOUBLE_EQ(J.coeff(2, 0),
//                    V2 * (Gt * cos(phi2 - phi1) + Bt * sin(phi2 - phi1)));
//   EXPECT_DOUBLE_EQ(J.coeff(2, 1),
//                    V1 * V2 * (Gt * sin(phi2 - phi1) - Bt * cos(phi2 - phi1)));

//   EXPECT_DOUBLE_EQ(J.coeff(2, 2), 2 * G2 * V2 + V1 * (Gt * cos(phi2 - phi1) +
//                                                       Bt * sin(phi2 - phi1)));

//   EXPECT_DOUBLE_EQ(J.coeff(2, 3),
//                    V2 * V1 * (-Gt * sin(phi2 - phi1) + Bt * cos(phi2 - phi1)));

//   EXPECT_DOUBLE_EQ(J.coeff(3, 2), 1.0);
// }
