#include "Matrixhandler.hpp"
#include "Net.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"
#include <Eigen/Sparse>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

//declarations:
nlohmann::json make_boundary(std::string id, Eigen::Vector2d condition);


// Test fixture.
// Defintions are at the bottom of the file.
class PowerTEST : public ::testing::Test {

public:
  PowerTEST() {
    set_default_net();
  }
  Network::Net net{std::vector<std::unique_ptr<Network::Node>>(), std::vector<std::unique_ptr<Network::Edge>>()};
  // values for the Vphinode:
  double V1_bd = 8.0;
  double phi1_bd = 6.0;
  double G1 = 3.0;
  double B1 = -5.0;

  // values for the PQnode:
  double P2_bd = 3.0;
  double Q2_bd = 7.0;
  double G2 = 2.0;
  double B2 = 2.0;

  // values for the PVnode:
  double P3_bd = 4.0;
  double V3_bd = 19.0;
  double G3 = 9.0;
  double B3 = 2.0;

  // Transmissionline from Vphi to PQ:
  double Gt1 = 2.0;
  double Bt1 = 4.0;

  // Transmissionline from PQ to PV:
  double Gt2 = 20.0;
  double Bt2 = 79.0;

  // default parameters for the state vector:
  double V1 = 1.0;
  double phi1 = 2.0;
  double V2 = 3.0;
  double phi2 = 4.0;
  double V3 = 5.0;
  double phi3 = 89.0;

  Eigen::VectorXd rootvalues{6};
  double last_time = 0.0;
  double new_time = 0.0;
  Eigen::VectorXd last_state{6};
  Eigen::VectorXd new_state{6};

 void set_default_net();

  void
  set_custom_net(Eigen::Vector2d vphi_par, Eigen::Vector2d pq_par,
             Eigen::Vector2d pv_par, Eigen::Vector2d tl_vphi_pq_par,
             Eigen::Vector2d tl_pq_pv_par, Eigen::Vector2d vphi_boundary,
             Eigen::Vector2d pq_boundary, Eigen::Vector2d pv_boundary);

  std::vector<Model::Networkproblem::Equationcomponent *>
  get_eq_components();

  void SetUp() override;

};


TEST_F(PowerTEST, evaluate_Vphi) {

  auto eqcomponents = get_eq_components();
  new_state << V1, phi1, V2, phi2, V3, phi3;

  for (auto &comp : eqcomponents) {
    comp->evaluate(rootvalues, last_time, new_time, last_state, new_state);
  }

  // Vphi node:
  EXPECT_DOUBLE_EQ(rootvalues[0], new_state[0] - V1_bd);
  EXPECT_DOUBLE_EQ(rootvalues[1], new_state[1] - phi1_bd);
}

TEST_F(PowerTEST, evaluate_state_derivative_Vphi) {

  auto eqcomponents = get_eq_components();
  new_state << V1, phi1, V2, phi2, V3, phi3;

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);
 
  for (auto &comp : eqcomponents) {
    comp->evaluate_state_derivative(&handler, last_time,  new_time, last_state, new_state);
  }
     handler.set_matrix();
  // Vphi node:
     Eigen::MatrixXd DenseJ = J;
     // derivatives of index 0, that is, V in the Vphi node:
     EXPECT_DOUBLE_EQ(DenseJ(0, 0), 1.0);
     EXPECT_DOUBLE_EQ(DenseJ(0, 1), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(0, 2), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(0, 3), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(0, 4), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(0, 5), 0.0);

     // derivatives of index 1, that is, phi in the Vphi node:
     EXPECT_DOUBLE_EQ(DenseJ(1, 0), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(1, 1), 1.0);
     EXPECT_DOUBLE_EQ(DenseJ(1, 2), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(1, 3), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(1, 4), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(1, 5), 0.0);
       }

TEST_F(PowerTEST, evaluate_PQ) {

  auto eqcomponents = get_eq_components();
  new_state << V1, phi1, V2, phi2, V3, phi3;

  for (auto &comp : eqcomponents) {
    comp->evaluate(rootvalues, last_time, new_time, last_state, new_state);
  }

    EXPECT_DOUBLE_EQ(rootvalues[2],
                     -P2_bd + G2 * V2 * V2 +
                         V2 * V1 *
                             (Gt1 * cos(phi2 - phi1) + Bt1 * sin(phi2 -
                             phi1)) +
                         V2 * V3 *
                             (Gt2 * cos(phi2 - phi3) + Bt2 * sin(phi2 -
                                                                 phi3)));
    EXPECT_DOUBLE_EQ(
        rootvalues[3],
        -Q2_bd - B2 * V2 * V2 +
            V2 * V1 * (Gt1 * sin(phi2 - phi1) - Bt1 * cos(phi2 - phi1)) +
            V2 * V3 * (Gt2 * sin(phi2 - phi3) - Bt2 * cos(phi2 - phi3)));
  }

TEST_F(PowerTEST, evaluate_state_derivative_PQ) {

  auto eqcomponents = get_eq_components();
  new_state << V1, phi1, V2, phi2, V3, phi3;

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);
 
  for (auto &comp : eqcomponents) {
    comp->evaluate_state_derivative(&handler, last_time,  new_time, last_state, new_state);
  }
     handler.set_matrix();
  // Vphi node:
     Eigen::MatrixXd DenseJ = J;

     // derivatives of index 2, that is, P in the PQ node:
     EXPECT_DOUBLE_EQ(DenseJ(2, 0),
                      V2 * (Gt1 * cos(phi2 - phi1) + Bt1 * sin(phi2 - phi1)));
     EXPECT_DOUBLE_EQ(DenseJ(2, 1),
                      V1 * V2 *
                          (Gt1 * sin(phi2 - phi1) - Bt1 * cos(phi2 - phi1)));
     EXPECT_DOUBLE_EQ(DenseJ(2, 2),
         2 * G2 * V2 + V1 * (Gt1 * cos(phi2 - phi1) + Bt1 * sin(phi2 - phi1)) +
             V3 * (Gt2 * cos(phi2 - phi3) + Bt2 * sin(phi2 - phi3)));
     EXPECT_DOUBLE_EQ(DenseJ(2, 3),
         V2 * V1 * (-Gt1 * sin(phi2 - phi1) + Bt1 * cos(phi2 - phi1)) +
             V2 * V3 * (-Gt2 * sin(phi2 - phi3) + Bt2 * cos(phi2 - phi3)));
     EXPECT_DOUBLE_EQ(DenseJ(2, 4),
                      V2 * (Gt2 * cos(phi2 - phi3) + Bt2 * sin(phi2 - phi3)));
     EXPECT_DOUBLE_EQ(DenseJ(2, 5),
                      V3 * V2 *
                          (Gt2 * sin(phi2 - phi3) - Bt2 * cos(phi2 - phi3)));

     // derivatives of index 3, that is, Q in the PQ node:
     EXPECT_DOUBLE_EQ(DenseJ(3, 0),
                      V2 * (Gt1 * sin(phi2 - phi1) - Bt1 * cos(phi2 - phi1)));
     EXPECT_DOUBLE_EQ(DenseJ(3, 1),
                      V2 * V1 *
                          (-Gt1 * cos(phi2 - phi1) - Bt1 * sin(phi2 - phi1)));
     EXPECT_DOUBLE_EQ(DenseJ(3, 2),
         -2 * B2 * V2 + V1 * (Gt1 * sin(phi2 - phi1) - Bt1 * cos(phi2 - phi1)) +
             V3 * (Gt2 * sin(phi2 - phi3) - Bt2 * cos(phi2 - phi3)));
     EXPECT_DOUBLE_EQ(
         DenseJ(3, 3),
         V2 * V1 * (Gt1 * cos(phi2 - phi1) + Bt1 * sin(phi2 - phi1)) +
             V2 * V3 * (Gt2 * cos(phi2 - phi3) + Bt2 * sin(phi2 - phi3)));
     EXPECT_DOUBLE_EQ(DenseJ(3, 4), V2 * (Gt2 * sin(phi2 - phi3) - Bt2 * cos(phi2 - phi3)));
     EXPECT_DOUBLE_EQ(DenseJ(3, 5),
                      V2 * V3 *
                          (-Gt2 * cos(phi2 - phi3) - Bt2 * sin(phi2 - phi3)));

}


TEST_F(PowerTEST, evaluate_PV) {

  auto eqcomponents = get_eq_components();
  new_state << V1, phi1, V2, phi2, V3, phi3;

  for (auto &comp : eqcomponents) {
    comp->evaluate(rootvalues, last_time, new_time, last_state, new_state);
  }

    EXPECT_DOUBLE_EQ(rootvalues[4],
                     -P3_bd + G3 * V3 * V3 +
                         V3 * V2 *
                             (Gt2 * cos(phi3 - phi2) + Bt2 * sin(phi3 -
                             phi2)));
    EXPECT_DOUBLE_EQ(rootvalues[5], new_state[4] - V3_bd);
  }

TEST_F(PowerTEST, evaluate_state_derivative_PV) {

  auto eqcomponents = get_eq_components();
  new_state << V1, phi1, V2, phi2, V3, phi3;

  Eigen::SparseMatrix<double> J(new_state.size(), new_state.size());
  Aux::Triplethandler handler(&J);
 
  for (auto &comp : eqcomponents) {
    comp->evaluate_state_derivative(&handler, last_time,  new_time, last_state, new_state);
  }
     handler.set_matrix();
     Eigen::MatrixXd DenseJ = J;

     //derivatives of index 4, that is, P in the PV node:
     EXPECT_DOUBLE_EQ(DenseJ(4, 0), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(4, 1), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(4, 2),
                      V3 * (Gt2 * cos(phi3 - phi2) + Bt2 * sin(phi3 - phi2)));
     EXPECT_DOUBLE_EQ(DenseJ(4, 3),
                      V2 * V3 *
                          (Gt2 * sin(phi3 - phi2) - Bt2 * cos(phi3 - phi2)));

     EXPECT_DOUBLE_EQ(
         DenseJ(4, 4),
         2 * G3 * V3 + V2 * (Gt2 * cos(phi3 - phi2) + Bt2 * sin(phi3 - phi2)));
     EXPECT_DOUBLE_EQ(
         DenseJ(4, 5),
         V3 * V2 * (-Gt2 * sin(phi3 - phi2) + Bt2 * cos(phi3 - phi2)));

     // derivatives of index 5, that is, V in the PV node:
     EXPECT_DOUBLE_EQ(DenseJ(5, 0), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(5, 1), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(5, 2), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(5, 3), 0.0);
     EXPECT_DOUBLE_EQ(DenseJ(5, 4), 1.0);
     EXPECT_DOUBLE_EQ(DenseJ(5, 5), 0.0);
     
}




//////////////////////////////////////////////////////
// Here come the definitions of the fixture methods:
/////////////////////////////////////////////////////

nlohmann::json make_boundary(std::string id, Eigen::Vector2d condition) {
  nlohmann::json bound;
  bound["id"] = id;
  bound["data"] = nlohmann::json::array();
  nlohmann::json b0;
  b0["time"] = 0;
  b0["values"] = {condition[0], condition[1]};
  bound["data"].push_back(b0);

  return bound;
}


void PowerTEST::set_custom_net(
    Eigen::Vector2d vphi_par, Eigen::Vector2d pq_par, Eigen::Vector2d pv_par,
    Eigen::Vector2d tl_vphi_pq_par, Eigen::Vector2d tl_pq_pv_par,
    Eigen::Vector2d vphi_boundary, Eigen::Vector2d pq_boundary,
    Eigen::Vector2d pv_boundary) {
  std::vector<std::unique_ptr<Network::Node>> nodes;
  std::vector<std::unique_ptr<Network::Edge>> edges;

  {
    nlohmann::json vphi_json;
    vphi_json["id"] = "vphi";
    vphi_json["G"] = vphi_par[0];
    vphi_json["B"] = vphi_par[1];
    auto b0 = make_boundary(vphi_json["id"], vphi_boundary);
    vphi_json["boundary_values"] = b0;

    nodes.push_back(
        std::make_unique<Model::Networkproblem::Power::Vphinode>(vphi_json));
  }
  {
    nlohmann::json pq_json;
    pq_json["id"] = "pq";
    pq_json["G"] = pq_par[0];
    pq_json["B"] = pq_par[1];

    auto b1 = make_boundary(pq_json["id"], pq_boundary);
    pq_json["boundary_values"] = b1;

    nodes.push_back(
        std::make_unique<Model::Networkproblem::Power::PQnode>(pq_json));
  }
  {
    nlohmann::json pv_json;
    pv_json["id"] = "pv";
    pv_json["G"] = pv_par[0];
    pv_json["B"] = pv_par[1];

    auto b2 = make_boundary(pv_json["id"], pv_boundary);
    pv_json["boundary_values"] = b2;

    nodes.push_back(
        std::make_unique<Model::Networkproblem::Power::PVnode>(pv_json));
  }
  {
    nlohmann::json tl_vphi_pq_json;
    tl_vphi_pq_json["id"] = "tl_vphi_pq";
    tl_vphi_pq_json["from"] = "vphi";
    tl_vphi_pq_json["to"] = "pq";
    tl_vphi_pq_json["G"] = tl_vphi_pq_par[0];
    tl_vphi_pq_json["B"] = tl_vphi_pq_par[1];

    edges.push_back(
        std::make_unique<Model::Networkproblem::Power::Transmissionline>(
            tl_vphi_pq_json, nodes));
  }

  {
    nlohmann::json tl_pq_pv_json;
    tl_pq_pv_json["id"] = "tl_pq_pv";
    tl_pq_pv_json["from"] = "pq";
    tl_pq_pv_json["to"] = "pv";
    tl_pq_pv_json["G"] = tl_pq_pv_par[0];
    tl_pq_pv_json["B"] = tl_pq_pv_par[1];

    edges.push_back(
        std::make_unique<Model::Networkproblem::Power::Transmissionline>(
            tl_pq_pv_json, nodes));
  }
  net = Network::Net(std::move(nodes), std::move(edges));
}

std::vector<Model::Networkproblem::Equationcomponent *>
PowerTEST::get_eq_components() {

  std::vector<Model::Networkproblem::Equationcomponent *> eqcomponents;
  for (auto &node : net.get_nodes()) {
    auto eqcomp =
        dynamic_cast<Model::Networkproblem::Equationcomponent *>(node);
    if (eqcomp) {
      eqcomponents.push_back(eqcomp);
    }
  }
  for (auto &edge : net.get_edges()) {
    auto eqcomp =
        dynamic_cast<Model::Networkproblem::Equationcomponent *>(edge);
    if (eqcomp) {
      eqcomponents.push_back(eqcomp);
    }
  }
  return eqcomponents;
}

void PowerTEST::SetUp(){
  int next_free_index = 0;
  auto eqcomponents = get_eq_components();
  for (auto &comp : eqcomponents) {
    next_free_index = comp->set_indices(next_free_index);
  }
  for (auto &comp : eqcomponents) {
    comp->setup();
  }
}


void PowerTEST::set_default_net() {

  Eigen::Vector2d vphi_param;
  vphi_param << G1, B1;
  Eigen::Vector2d pq_param;
  pq_param << G2, B2;
  Eigen::Vector2d pv_param;
  pv_param << G3, B3;

  Eigen::Vector2d tl_vphi_pq_param;
  tl_vphi_pq_param << Gt1, Bt1;

  Eigen::Vector2d tl_pq_pv_param;
  tl_pq_pv_param << Gt2, Bt2;

  Eigen::Vector2d vphi_boundary;
  vphi_boundary << V1_bd, phi1_bd;
  Eigen::Vector2d pq_boundary;
  pq_boundary << P2_bd, Q2_bd;

  Eigen::Vector2d pv_boundary;
  pv_boundary << P3_bd, V3_bd;

  set_custom_net(vphi_param, pq_param, pv_param, tl_vphi_pq_param,
                    tl_pq_pv_param, vphi_boundary, pq_boundary, pv_boundary);
}