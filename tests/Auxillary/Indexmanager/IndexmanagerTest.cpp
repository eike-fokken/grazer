#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

#include "Indexmanager.hpp"
#include "InterpolatingVector.hpp"
#include "Timedata.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"
#include <functional>

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

TEST(Timeless_Indexmanager, set_initial_values_happy_simple) {

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

TEST(Timeless_Indexmanager, set_initial_values_happy_delta_x) {

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
           {{"x", 0.5}, {"values", {b(0), b(1)}}}}}};

  std::vector<nlohmann::json> contains_x
      = {R"({"minimum": 0, "maximum": 0})"_json,
         R"({"minimum": 0.5, "maximum": 0.5})"_json};

  nlohmann::json initial_schema
      = Aux::schema::make_initial_schema(2, 2, contains_x);

  class Testpipe {
  public:
    Eigen::Vector2d test_conversion(Eigen::Vector2d x) {
      Eigen::Vector2d y;
      y << x[1], x[0];
      return y;
    }
  } testpipe;

  auto func
      = std::bind(&Testpipe::test_conversion, testpipe, std::placeholders::_1);
  manager.set_initial_values(
      vector_to_be_filled, number_of_points, initial_json, initial_schema, 0.5,
      func);

  EXPECT_EQ(testpipe.test_conversion(a), vector_to_be_filled.segment<2>(0));
  EXPECT_EQ(testpipe.test_conversion(b), vector_to_be_filled.segment<2>(2));
}

TEST(Timed_Indexmanager, set_initial_values_happy) {

  nlohmann::json timedata_json = R"(    {
        "use_simplified_newton": true,
        "maximal_number_of_newton_iterations": 100,
        "tolerance": 1e-8,
        "retries": 0,
        "start_time": 0,
        "end_time": 10,
        "desired_delta_t": 0.5
    }
)"_json;

  Model::Timedata timedata(timedata_json);

  Timed_Indexmanager<2> manager;

  manager.set_indices(0, 38);

  nlohmann::json control_schema = Aux::schema::make_boundary_schema(2);
  nlohmann::json control_json = R"({
				"id": "testid",
				"data": [
					{
						"time": 0,
						"values": [
							0.0,
              0.0
						]
					},
					{
						"time": 10,
						"values": [
							10,
              20
						]
					}
				]
			})"_json;

  auto data = Aux::interpolation_points_helper(0, 0.5, 10);
  Aux::InterpolatingVector::construct_from_json(control_json["data"]);
  Aux::InterpolatingVector controller(data, 2);

  manager.set_initial_values(
      timedata, controller, control_json, control_schema);
}
