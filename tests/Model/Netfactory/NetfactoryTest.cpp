#include "Netfactory.hpp"
#include "ComponentJsonHelpers.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Power_factory.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

#include <exception>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

TEST(sort_json_vectors_by_idTEST, not_sorted) {

  nlohmann::json topology_not_sorted;
  nlohmann::json topology_sorted;

  topology_not_sorted
      = {{"nodes",
          {{"Source",
            {{{"id", "node_3"}, {"x", 10.000000}},
             {{"id", "node_2"}, {"x", 12.000000}},
             {{"id", "node_4"}, {"x", 13.000000}},
             {{"id", "node_1"}, {"x", 13.000000}}}},
           {"Innode",
            {{{"id", "node_6"}, {"x", 10.000000}},
             {{"id", "node_5"}, {"x", 12.000000}},
             {{"id", "node_8"}, {"x", 13.000000}},
             {{"id", "node_7"}, {"x", 13.000000}}}}}},
         {"connections",
          {{"Gaspowerconnection",
            {{{"id", "nodeld10"}, {"to", "N7071"}},
             {{"id", "nodeld57"}, {"to", "N7057"}},
             {{"id", "nodeld31"}, {"to", "N2131"}},
             {{"id", "nodeld80"}, {"to", "N3421"}}}},
           {"Transmissionline",
            {{{"id", "TL_1_3"}, {"B", 20}},
             {{"id", "TL_2_3"}, {"B", 30}},
             {{"id", "TL_1_5"}, {"B", 10}},
             {{"id", "TL_3_6"}, {"B", 50}}}}}}};

  topology_sorted
      = {{"nodes",
          {{"Source",
            {{{"id", "node_1"}, {"x", 13.000000}},
             {{"id", "node_2"}, {"x", 12.000000}},
             {{"id", "node_3"}, {"x", 10.000000}},
             {{"id", "node_4"}, {"x", 13.000000}}}},
           {"Innode",
            {{{"id", "node_5"}, {"x", 12.000000}},
             {{"id", "node_6"}, {"x", 10.000000}},
             {{"id", "node_7"}, {"x", 13.000000}},
             {{"id", "node_8"}, {"x", 13.000000}}}}}},
         {"connections",
          {{"Gaspowerconnection",
            {{{"id", "nodeld10"}, {"to", "N7071"}},
             {{"id", "nodeld31"}, {"to", "N2131"}},
             {{"id", "nodeld57"}, {"to", "N7057"}},
             {{"id", "nodeld80"}, {"to", "N3421"}}}},
           {"Transmissionline",
            {{{"id", "TL_1_3"}, {"B", 20}},
             {{"id", "TL_1_5"}, {"B", 10}},
             {{"id", "TL_2_3"}, {"B", 30}},
             {{"id", "TL_3_6"}, {"B", 50}}}}}}};

  Aux::sort_json_vectors_by_id(topology_not_sorted, "topology_key");
  EXPECT_EQ(topology_not_sorted, topology_sorted);
}

TEST(check_for_duplicatesTEST, duplicate_id_in_same_vector) {

  nlohmann::json id_duplicate_json;

  id_duplicate_json
      = {{"nodes",
          {{"Source",
            {{{"id", "node_1"}, {"x", 10.000000}}, // "node_1" is a duplicate id
             {{"id", "node_3"}, {"x", 12.000000}},
             {{"id", "node_4"}, {"x", 13.000000}},
             {{"id", "node_1"}, {"x", 13.000000}}}},
           {"Innode",
            {{{"id", "node_5"}, {"x", 10.000000}},
             {{"id", "node_6"}, {"x", 13.000000}}}}}}};

  try {
    Aux::check_for_duplicates(id_duplicate_json, "topology_key");
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr("The id node_1 appears twice in topology_key"));
  }
}

TEST(check_for_duplicatesTEST, duplicate_id_in_neighbour_vector) {

  nlohmann::json id_duplicate_json;

  id_duplicate_json = {
      {"nodes",
       {{"Source",
         {{{"id", "node_1"}, {"x", 10.000000}},
          {{"id", "node_3"}, {"x", 12.000000}},
          {{"id", "node_2"}, {"x", 13.000000}},
          {{"id", "node_4"}, {"x", 13.000000}}}}, // "node 4" is a duplicate id
        {"Innode",
         {{{"id", "node_4"}, {"x", 10.000000}},
          {{"id", "node_6"}, {"x", 13.000000}}}}}}};

  try {
    Aux::check_for_duplicates(id_duplicate_json, "topology_key");
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr("The id node_4 appears twice in topology_key"));
  }
}

TEST(build_node_vectorTEST, node_type_not_known) {

  Model::Componentfactory::Power_factory power_factory{R"({})"_json};
  auto &nodetypemap = power_factory.node_type_map;

  nlohmann::json node_topology;

  node_topology
      = {{"Vphinode",
          {{{"B", -25.2023789002},
            {"G", 1.7238407171},
            {"id", "N201"},
            {"boundary_values",
             {{"id", "N201"},
              {"data", {{{"time", 1}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PVnode",
          {{{"B", -29.2023789002},
            {"G", 1.7238407171},
            {"id", "N202"},
            {"boundary_values",
             {{"id", "N202"},
              {"data", {{{"time", 1}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PQnode",
          {{{"B", -16.2023789002},
            {"G", 1.7238407171},
            {"id", "N203"},
            {"boundary_values",
             {{"id", "N203"},
              {"data", {{{"time", 1}, {"values", {1.01, -0.203}}}}}}}}}},
         {"Unknown_type_node",
          {{"B", -26.2023789002},
           {"G", 1.7238407171},
           {"id", "N204"},
           {"boundary_values",
            {{"id", "N204"},
             {"data", {{{{"time", 1}, {"values", {1.01, -0.203}}}}}}}}}}};

  try {
    Model::build_node_vector(node_topology, nodetypemap);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr("node type Unknown_type_node, given in the topology "
                           "file, is unknown to grazer"));
  }
}

TEST(build_node_vectorTEST, evaluate) {

  Model::Componentfactory::Power_factory power_factory(R"({})"_json);
  auto &nodetypemap = power_factory.node_type_map;

  nlohmann::json node_topology;

  node_topology
      = {{"Vphinode",
          {{{"B", -25.2023789002},
            {"G", 1.7238407171},
            {"id", "N201"},
            {"boundary_values",
             {{"id", "N201"},
              {"data", {{{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PVnode",
          {{{"B", -29.2023789002},
            {"G", 1.7238407171},
            {"id", "N202"},
            {"boundary_values",
             {{"id", "N202"},
              {"data", {{{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PQnode",
          {{{"B", -16.2023789002},
            {"G", 1.7238407171},
            {"id", "N203"},
            {"boundary_values",
             {{"id", "N203"},
              {"data", {{{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}}};

  std::vector<std::unique_ptr<Network::Node>> nodes;
  nodes = Model::build_node_vector(node_topology, nodetypemap);

  EXPECT_TRUE(dynamic_cast<Model::Power::PQnode *>(nodes[0].get()));
  EXPECT_TRUE(dynamic_cast<Model::Power::PVnode *>(nodes[1].get()));
  EXPECT_TRUE(dynamic_cast<Model::Power::Vphinode *>(nodes[2].get()));
}

TEST(build_edge_vectorTEST, edge_type_not_known) {

  // Building a node vector is required in order to test build_edge_vector()

  Model::Componentfactory::Power_factory power_factory(R"({})"_json);
  auto &nodetypemap = power_factory.node_type_map;

  nlohmann::json node_topology;

  node_topology
      = {{"Vphinode",
          {{{"B", -25.2023789002},
            {"G", 1.7238407171},
            {"id", "N201"},
            {"boundary_values",
             {{"id", "N201"},
              {"data", {{{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PVnode",
          {{{"B", -29.2023789002},
            {"G", 1.7238407171},
            {"id", "N202"},
            {"boundary_values",
             {{"id", "N202"},
              {"data", {{{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PQnode",
          {{{"B", -16.2023789002},
            {"G", 1.7238407171},
            {"id", "N203"},
            {"boundary_values",
             {{"id", "N203"},
              {"data", {{{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}}};

  std::vector<std::unique_ptr<Network::Node>> nodes;
  nodes = Model::build_node_vector(node_topology, nodetypemap);

  auto &edgetypemap = power_factory.edge_type_map;
  nlohmann::json edge_topology;

  edge_topology
      = {{"Unknown_type_edge",
          {{{"from", "N203"},
            {"to", "N202"},
            {"B", 1},
            {"G", 1},
            {"id", "N201"},
            {"power2gas_q_coeff", 1},
            {"gas2power_q_coeff", 1}},
           {{"from", "N201"},
            {"to", "N202"},
            {"B", 2},
            {"G", 2},
            {"id", "N203"},
            {"power2gas_q_coeff", 0.5},
            {"gas2power_q_coeff", 0.5}}}}};

  try {
    Model::build_edge_vector(edge_topology, nodes, edgetypemap);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr("edge type Unknown_type_edge, given in the topology "
                           "file, is unknown to grazer"));
  }
}

TEST(build_edge_vectorTEST, evaluate) {

  Model::Componentfactory::Power_factory power_factory(R"({})"_json);
  auto &nodetypemap = power_factory.node_type_map;

  nlohmann::json node_topology;

  node_topology
      = {{"Vphinode",
          {{{"B", -25.2023789002},
            {"G", 1.7238407171},
            {"id", "N201"},
            {"boundary_values",
             {{"id", "N201"},
              {"data", {{{"time", 1}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PVnode",
          {{{"B", -29.2023789002},
            {"G", 1.7238407171},
            {"id", "N202"},
            {"boundary_values",
             {{"id", "N202"},
              {"data", {{{"time", 1}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PQnode",
          {{{"B", -16.2023789002},
            {"G", 1.7238407171},
            {"id", "N203"},
            {"boundary_values",
             {{"id", "N203"},
              {"data", {{{"time", 1}, {"values", {1.01, -0.203}}}}}}}}}}};

  std::vector<std::unique_ptr<Network::Node>> nodes;
  nodes = Model::build_node_vector(node_topology, nodetypemap);

  auto &edgetypemap = power_factory.edge_type_map;
  nlohmann::json edge_topology;

  edge_topology
      = {{"Transmissionline",
          {{{"from", "N203"},
            {"to", "N202"},
            {"B", 1},
            {"G", 1},
            {"id", "N201"},
            {"power2gas_q_coeff", 1},
            {"gas2power_q_coeff", 1}},
           {{"from", "N201"},
            {"to", "N202"},
            {"B", 2},
            {"G", 2},
            {"id", "N203"},
            {"power2gas_q_coeff", 0.5},
            {"gas2power_q_coeff", 0.5}}}}};

  std::vector<std::unique_ptr<Network::Edge>> edges;
  edges = Model::build_edge_vector(edge_topology, nodes, edgetypemap);

  EXPECT_TRUE(dynamic_cast<Model::Power::Transmissionline *>(edges[0].get()));
}
