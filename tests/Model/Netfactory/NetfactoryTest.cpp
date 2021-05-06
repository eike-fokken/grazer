#include "Netfactory.hpp"
#include <Eigen/Dense>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

TEST(sort_json_vectors_by_idTEST, not_sorted) {

  nlohmann::json topology_not_sorted;
  nlohmann::json topology_sorted;

  topology_not_sorted = {
    {"nodes",
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
      {{{"id", "nodeld10"},{"to","N7071"}},
       {{"id", "nodeld57"},{"to","N7057"}},
       {{"id", "nodeld31"},{"to","N2131"}},
       {{"id", "nodeld80"},{"to","N3421"}}}},
      {"Transmissionline",
      {{{"id", "TL_1_3"},{"B",20}},
       {{"id", "TL_2_3"},{"B",30}},
       {{"id", "TL_1_5"},{"B",10}},
       {{"id", "TL_3_6"},{"B",50}}}}}}};

  topology_sorted = {
    {"nodes",
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
      {{{"id", "nodeld10"},{"to","N7071"}},
       {{"id", "nodeld31"},{"to","N2131"}},
       {{"id", "nodeld57"},{"to","N7057"}},
       {{"id", "nodeld80"},{"to","N3421"}}}},
     {"Transmissionline",
      {{{"id", "TL_1_3"},{"B",20}},
       {{"id", "TL_1_5"},{"B",10}},
       {{"id", "TL_2_3"},{"B",30}},
       {{"id", "TL_3_6"},{"B",50}}}}}}};

  Model::Networkproblem::sort_json_vectors_by_id(topology_not_sorted, "topology_key");
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

  EXPECT_THROW(Model::Networkproblem::check_for_duplicates(
               id_duplicate_json, "topology_key"), std::runtime_error);

}

TEST(check_for_duplicatesTEST, duplicate_id_in_neighbour_vector) {

  nlohmann::json id_duplicate_json;

  id_duplicate_json
      = {{"nodes",
          {{"Source",
            {{{"id", "node_1"}, {"x", 10.000000}}, // "node_1" is a duplicate id
             {{"id", "node_3"}, {"x", 12.000000}},
             {{"id", "node_2"}, {"x", 13.000000}},
             {{"id", "node_4"}, {"x", 13.000000}}}},
           {"Innode",
            {{{"id", "node_4"}, {"x", 10.000000}},
             {{"id", "node_6"}, {"x", 13.000000}}}}}}};

  EXPECT_THROW(
      Model::Networkproblem::check_for_duplicates(
          id_duplicate_json, "topology_key"),
      std::runtime_error);
}
