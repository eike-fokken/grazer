#include "Netfactory.hpp"
#include "PQnode.hpp"
#include "PVnode.hpp"
#include "Power_factory.hpp"
#include "Transmissionline.hpp"
#include "Vphinode.hpp"

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

  Model::Networkproblem::sort_json_vectors_by_id(
      topology_not_sorted, "topology_key");
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

  EXPECT_THROW(
      Model::Networkproblem::check_for_duplicates(
          id_duplicate_json, "topology_key"),
      std::runtime_error);
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

  EXPECT_THROW(
      Model::Networkproblem::check_for_duplicates(
          id_duplicate_json, "topology_key"),
      std::runtime_error);
}

TEST(build_node_vectorTEST, node_type_not_known) {

  Model::Componentfactory::Power_factory power_factory;
  auto &nodetypemap = power_factory.node_type_map;

  nlohmann::json node_topology;

  node_topology
      = {{"Vphinode",
          {{{"B", -25.2023789002},
            {"G", 1.7238407171},
            {"id", "N201"},
            {"boundary_values",
             {{"id", "N201"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PVnode",
          {{{"B", -29.2023789002},
            {"G", 1.7238407171},
            {"id", "N202"},
            {"boundary_values",
             {{"id", "N202"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PQnode",
          {{{"B", -16.2023789002},
            {"G", 1.7238407171},
            {"id", "N203"},
            {"boundary_values",
             {{"id", "N203"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"wrong_node_type_name",
          {{"B", -26.2023789002},
           {"G", 1.7238407171},
           {"id", "N204"},
           {"boundary_values",
            {{"id", "N204"},
             {"data",
              {{{"time", 0}, {"values", {1.01, -0.203}}},
               {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}};

  EXPECT_THROW(
      Model::Networkproblem::build_node_vector(node_topology, nodetypemap),
      std::runtime_error);
}

TEST(build_node_vectorTEST, evaluate) {

  Model::Componentfactory::Power_factory power_factory;
  auto &nodetypemap = power_factory.node_type_map;

  nlohmann::json node_topology;

  node_topology
      = {{"Vphinode",
          {{{"B", -25.2023789002},
            {"G", 1.7238407171},
            {"id", "N201"},
            {"boundary_values",
             {{"id", "N201"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PVnode",
          {{{"B", -29.2023789002},
            {"G", 1.7238407171},
            {"id", "N202"},
            {"boundary_values",
             {{"id", "N202"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PQnode",
          {{{"B", -16.2023789002},
            {"G", 1.7238407171},
            {"id", "N203"},
            {"boundary_values",
             {{"id", "N203"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}}};

  std::vector<std::unique_ptr<Network::Node>> nodes;
  nodes = Model::Networkproblem::build_node_vector(node_topology, nodetypemap);

  EXPECT_TRUE(
      dynamic_cast<Model::Networkproblem::Power::PQnode *>(nodes[0].get()));
  EXPECT_TRUE(
      dynamic_cast<Model::Networkproblem::Power::PVnode *>(nodes[1].get()));
  EXPECT_TRUE(
      dynamic_cast<Model::Networkproblem::Power::Vphinode *>(nodes[2].get()));
}

TEST(build_edge_vectorTEST, edge_type_not_known) {

  // Building a node vector is required in order to test build_edge_vector()

  Model::Componentfactory::Power_factory power_factory;
  auto &nodetypemap = power_factory.node_type_map;

  nlohmann::json node_topology;

  node_topology
      = {{"Vphinode",
          {{{"B", -25.2023789002},
            {"G", 1.7238407171},
            {"id", "N201"},
            {"boundary_values",
             {{"id", "N201"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PVnode",
          {{{"B", -29.2023789002},
            {"G", 1.7238407171},
            {"id", "N202"},
            {"boundary_values",
             {{"id", "N202"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PQnode",
          {{{"B", -16.2023789002},
            {"G", 1.7238407171},
            {"id", "N203"},
            {"boundary_values",
             {{"id", "N203"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}}};

  std::vector<std::unique_ptr<Network::Node>> nodes;
  nodes = Model::Networkproblem::build_node_vector(node_topology, nodetypemap);

  auto &edgetypemap = power_factory.edge_type_map;
  nlohmann::json edge_topology;

  edge_topology
      = {{"wrong_edge_type_name",
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

  EXPECT_THROW(
      Model::Networkproblem::build_edge_vector(
          edge_topology, nodes, edgetypemap),
      std::runtime_error);
}

TEST(build_edge_vectorTEST, evaluate) {

  Model::Componentfactory::Power_factory power_factory;
  auto &nodetypemap = power_factory.node_type_map;

  nlohmann::json node_topology;

  node_topology
      = {{"Vphinode",
          {{{"B", -25.2023789002},
            {"G", 1.7238407171},
            {"id", "N201"},
            {"boundary_values",
             {{"id", "N201"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PVnode",
          {{{"B", -29.2023789002},
            {"G", 1.7238407171},
            {"id", "N202"},
            {"boundary_values",
             {{"id", "N202"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}},
         {"PQnode",
          {{{"B", -16.2023789002},
            {"G", 1.7238407171},
            {"id", "N203"},
            {"boundary_values",
             {{"id", "N203"},
              {"data",
               {{{"time", 0}, {"values", {1.01, -0.203}}},
                {{"time", 0}, {"values", {1.01, -0.203}}}}}}}}}}};

  std::vector<std::unique_ptr<Network::Node>> nodes;
  nodes = Model::Networkproblem::build_node_vector(node_topology, nodetypemap);

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
  edges = Model::Networkproblem::build_edge_vector(
      edge_topology, nodes, edgetypemap);

  EXPECT_TRUE(dynamic_cast<Model::Networkproblem::Power::Transmissionline *>(
      edges[0].get()));
}
