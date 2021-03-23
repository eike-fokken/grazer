#include "Gaspowerconnection.hpp"
#include "Equationcomponent_test_helpers.hpp"
#include "Innode.hpp"
#include "Matrixhandler.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "Full_factory.hpp"
#include "Vphinode.hpp"
#include <Eigen/src/Core/Matrix.h>
#include <gtest/gtest.h>

class GaspowerconnectionTEST : public ::testing::Test {
public:
  std::string output;

  std::unique_ptr<Model::Networkproblem::Networkproblem>
  get_Networkproblem(nlohmann::json &netproblem) {
    std::unique_ptr<Model::Networkproblem::Networkproblem> netprob;
    {
      std::stringstream buffer;
      Catch_cout catcher(buffer.rdbuf());
      auto net_ptr = Model::Networkproblem::build_net<
          Model::Componentfactory::Full_factory>(netproblem);
      netprob = std::make_unique<Model::Networkproblem::Networkproblem>(
          std::move(net_ptr));
      output = buffer.str();
    }

    return netprob;
  }
};

nlohmann::json gaspowerconnection_json(
    std::string id, nlohmann::json startnode, nlohmann::json endnode,
    double gas2power_q_coefficient,
    double power2gas_q_coefficient);

nlohmann::json vphinode_json(std::string id, double G, double B,
                             Eigen::Vector2d cond0, Eigen::Vector2d cond1);

nlohmann::json gaspowerconnection_json(std::string id, nlohmann::json startnode,
                                       nlohmann::json endnode,
                                       double gas2power_q_coefficient,
                                       double power2gas_q_coefficient){
  nlohmann::json gp_topology;
  gp_topology["id"] = id;
  gp_topology["from"] = startnode["id"];
  gp_topology["to"] = endnode["id"];
  gp_topology["gas2power_q_coeff"] = gas2power_q_coefficient;
  gp_topology["power2gas_q_coeff"] = power2gas_q_coefficient;
  return gp_topology;
}

nlohmann::json vphinode_json(std::string id, double G, double B,
                             Eigen::Vector2d cond0, Eigen::Vector2d cond1) {
  nlohmann::json vphi_topology;
  vphi_topology["id"] = id;
  vphi_topology["G"] = G;
  vphi_topology["B"] = B;
  auto bound = make_value_json(id, "time",cond0,cond1);
  vphi_topology["boundary_values"] = bound;
  return vphi_topology;
}


TEST_F(GaspowerconnectionTEST, evaluate){

  nlohmann::json innode_json;
  innode_json["id"] = "innode";
  double G = 4;
  double B = 123;
  Eigen::Vector2d cond0(345,333);
  Eigen::Vector2d cond1(341, 3331);
  auto vphi_json = vphinode_json("vphi", G, B, cond0, cond1);

  double gas2power_q_coefficient = 34;
  double power2gas_q_coefficient = 55;

  auto gp_json = gaspowerconnection_json("gp", innode_json, vphi_json,  gas2power_q_coefficient, power2gas_q_coefficient);

  auto netprob_json =
      make_full_json({{"Innode", {innode_json}}, {"Vphinode", {vphi_json}}},
                     {{"Gaspowerconnection",{gp_json}}});
  auto netprob = get_Networkproblem(netprob_json);
  auto number_of_variables = netprob->set_indices(0);

  double last_time = 0.0;
  double new_time = 1.0;
  Eigen::VectorXd rootvalues(number_of_variables);
  rootvalues.setZero();
  Eigen::VectorXd last_state(number_of_variables);
  Eigen::VectorXd new_state(number_of_variables);
  last_state.setRandom();

  double initial_pressure = 18;
  double initial_flow = -45;
  Eigen::Vector2d cond(initial_pressure, initial_flow);
  std::vector<std::pair<double, Eigen::Vector2d>> initial_vector({{0.0, cond}});
  auto gp_initial = make_value_json("gp", "x", initial_vector);
  auto net_initial =
      make_initial_json({}, {{"Gaspowerconnection", {gp_initial}}});
  netprob->set_initial_values(new_state, net_initial);

  netprob->evaluate(rootvalues, last_time, new_time, last_state, new_state);
  std::cout << rootvalues << std::endl;
}
