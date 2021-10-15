#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "Indexmanager.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

using namespace Aux;

class Indexmanager_tester : public Indexmanager {};

TEST(Indexmanager, set_indices_happy) {

  Indexmanager_tester manager;

  int startindex = 7;
  int number_of_indices = 3;
  manager.set_indices(startindex, number_of_indices);

  EXPECT_EQ(manager.get_startindex(), startindex);
  EXPECT_EQ(manager.get_afterindex(), startindex + number_of_indices);
}

TEST(Indexmanager, get_startindex_uninitialized) {

  Indexmanager_tester manager;

  try {
    manager.get_startindex();
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr(
                      "startindex < 0. Probably get_startindex() was called "));
  }
}

TEST(Indexmanager, get_afterindex_uninitialized) {

  Indexmanager_tester manager;

  try {
    manager.get_afterindex();
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr(
                      "afterindex < 0. Probably get_afterindex() was called "));
  }
}

TEST(Indexmanager, set_indices_negative_startindex) {

  Indexmanager_tester manager;

  int startindex = -1;
  int number_of_indices = 3;
  try {
    manager.set_indices(startindex, number_of_indices);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(e.what(), testing::HasSubstr("Tried to set negative index"));
  }
}

TEST(Indexmanager, set_indices_negative_number) {

  Indexmanager_tester manager;

  int startindex = 4;
  int number_of_indices = -13;
  try {
    manager.set_indices(startindex, number_of_indices);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr("Tried to reserve zero or less number of indices"));
  }
}

TEST(Timeless_Indexmanager, set_initial_values_happy) {

  Timeless_Indexmanager<2> manager;
  manager.set_indices(0, 4);

  Eigen::Vector2d a(0.0, 1.0);
  Eigen::Vector2d b(1.0, 2.0);

  Eigen::Vector4d vector_to_be_filled;
  int number_of_points = 2;

  nlohmann::json initial_json
      = {{"id", "N213"},
         {"data",
          {{{"x", 0.0}, {"values", {a(0), a(1)}}},
           {{"x", 1.0}, {"values", {b(0), b(1)}}}}}};

  std::vector<nlohmann::json> contains_x
      = {R"({"minimum": 0, "maximum": 0})"_json,
         R"({"minimum": 1, "maximum": 1})"_json};

  nlohmann::json initial_schema
      = Aux::schema::make_initial_schema(2, 2, contains_x);

  manager.set_initial_values(
      vector_to_be_filled, number_of_points, initial_json, initial_schema);

  EXPECT_EQ(a, vector_to_be_filled.segment<2>(0));
  EXPECT_EQ(b, vector_to_be_filled.segment<2>(2));
}
