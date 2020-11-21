#include "PQnode.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"
#include <Eigen/Dense>
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

TEST(testPower, test_P) {
  double G1 = 1.0;
  double B1 = 1.0;

  double G2 = 2.0;
  double B2 = 2.0;

  double Gt = 3.0;
  double Bt = 3.0;

  //   nlohmann::ordered_json j2 = {
  //   {"pi", 3.141},
  //   {"happy", true},
  //   {"name", "Niels"},
  //   {"nothing", nullptr},
  //   {"answer", {
  //     {"everything", 42}
  //   }},
  //   {"list", {1, 0, 2}},
  //   {"object", {
  //     {"currency", "USD"},
  //     {"value", 42.99}
  //   }}
  // };

  json bd_json1 = {
      {"id", "N1"},
      {"type", "Vphi"},
      {"data",
       json::array({{{"time", 0.}, {"values", json::array({1., 1.})}}})}};
  json bd_json2 = {
      {"id", "N2"},
      {"type", "Vphi"},
      {"data",
       json::array({{{"time", 0.}, {"values", json::array({2., 2.})}}})}};
  std::cout << bd_json2 << std::endl;

  Model::Networkproblem::Power::Vphinode n1("N1", bd_json1, G1, B1);
  Model::Networkproblem::Power::PQnode n2("N2", bd_json2, G2, B2);
  Model::Networkproblem::Power::Transmissionline t("T1", &n1, &n2, Gt, Bt);
  n1.attach_starting_edge(&t);
  n2.attach_starting_edge(&t);

  std::cout << bd_json1 << std::endl;
};
