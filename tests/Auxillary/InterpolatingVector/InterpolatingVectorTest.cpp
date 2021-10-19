#include "InterpolatingVector.hpp"

#include "Exception.hpp"
#include "make_schema.hpp"
#include <Eigen/src/Core/Matrix.h>
#include <cstddef>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Aux;

TEST(InterpolatingVector, Construction_happy_path) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::interpolation_points_helper(start, delta, number_of_points);
  InterpolatingVector interpolatingvector(data, number_of_values_per_point);

  EXPECT_EQ(
      interpolatingvector.get_total_number_of_values(),
      number_of_values_per_point * number_of_points);
}

TEST(InterpolatingVector, Construction_negative_number_of_controls) {

  int number_of_values_per_point = -1;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::interpolation_points_helper(start, delta, number_of_points);

  try {
    InterpolatingVector interpolatingvector(data, number_of_values_per_point);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr(
            "You can't have less than 0 entries per interpolation point"));
  }
}

// Must become test of Interpolation_data
// TEST(InterpolatingVector, Construction_nonpositive_number_steps) {

//   int number_of_values_per_point = -1;
//   int number_of_points = 8;
//   double start = 0;
//   double delta = 0.5;
//   auto data = Aux::interpolation_points_helper(start, delta,
//   number_of_points);

//   try {
//     InterpolatingVector interpolatingvector(data,
//     number_of_values_per_point);
//       FAIL() << "Test FAILED: The statement ABOVE\n"
//              << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
//     } catch (std::runtime_error &e) {
//       EXPECT_THAT(
//           e.what(),
//           testing::HasSubstr("You can't have less than 1 time step!"));
//     }
//   }

TEST(InterpolatingVector, set_and_evaluate_controls_happy_path) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::interpolation_points_helper(start, delta, number_of_points);
  InterpolatingVector interpolatingvector(data, number_of_values_per_point);
  Eigen::VectorXd controls(interpolatingvector.get_total_number_of_values());

  for (int index = 0; index != controls.size(); ++index) {
    controls[index] = index;
  }
  interpolatingvector.set_values_in_bulk(controls);

  EXPECT_EQ(interpolatingvector.get_allvalues(), controls);

  for (Eigen::Index i = 0; i != number_of_points; ++i) {
    Eigen::VectorXd a
        = interpolatingvector(start + static_cast<double>(i) * delta);
    for (Eigen::Index j = 0; j != a.size(); ++j) {
      EXPECT_DOUBLE_EQ(a[j], controls[i * number_of_values_per_point + j]);
    }
  }
}

// TEST(InterpolatingVector, set_controls_invalid) {
//   int number_of_values_per_point = 4;
//   int number_of_entries = 8;

//   InterpolatingVector interpolatingvector(
//       number_of_values_per_point, number_of_entries);

//   Eigen::VectorXd controls(
//       interpolatingvector.get_total_number_of_values() - 1);

//   try {
//     interpolatingvector.set_values_in_bulk(controls);
//     FAIL() << "Test FAILED: The statement ABOVE\n"
//            << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
//   } catch (std::runtime_error &e) {
//     EXPECT_THAT(
//         e.what(),
//         testing::HasSubstr(
//             "You are trying to assign the wrong number of controls."));
//   }
// }

// TEST(InterpolatingVector, evaluate_controls_invalid_high_index) {

//   int number_of_values_per_point = 8;
//   int number_of_entries = 88;

//   InterpolatingVector interpolatingvector(
//       number_of_values_per_point, number_of_entries);

//   Eigen::VectorXd controls(interpolatingvector.get_total_number_of_values());

//   for (int index = 0; index != controls.size(); ++index) {
//     controls[index] = index;
//   }
//   interpolatingvector.set_values_in_bulk(controls);

//   try {
//     interpolatingvector(number_of_values_per_point * number_of_entries);
//     FAIL() << "Test FAILED: The statement ABOVE\n"
//            << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
//   } catch (std::runtime_error &e) {
//     EXPECT_THAT(
//         e.what(), testing::HasSubstr(
//                       "You request controls at a higher index than
//                       possible!"));
//   }
// }

// TEST(InterpolatingVector, evaluate_controls_invalid_low_index) {

//   int number_of_values_per_point = 8;
//   int number_of_entries = 88;

//   InterpolatingVector interpolatingvector(
//       number_of_values_per_point, number_of_entries);

//   Eigen::VectorXd controls(interpolatingvector.get_total_number_of_values());

//   for (int index = 0; index != controls.size(); ++index) {
//     controls[index] = index;
//   }
//   interpolatingvector.set_values_in_bulk(controls);

//   try {
//     interpolatingvector(-1);
//     FAIL() << "Test FAILED: The statement ABOVE\n"
//            << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
//   } catch (std::runtime_error &e) {
//     EXPECT_THAT(
//         e.what(),
//         testing::HasSubstr("You request controls at negative time steps!"));
//   }
// }

TEST(InterpolatingVector, mut_timestep_happy_path) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::interpolation_points_helper(start, delta, number_of_points);
  InterpolatingVector interpolatingvector(data, number_of_values_per_point);

  Eigen::VectorXd controls(interpolatingvector.get_total_number_of_values());

  for (int index = 0; index != controls.size(); ++index) {
    controls[index] = index;
  }
  interpolatingvector.set_values_in_bulk(controls);

  for (int i = 0; i != number_of_points; ++i) {
    interpolatingvector.mut_timestep(i) = controls.segment(
        i * number_of_values_per_point, number_of_values_per_point);
  }
  EXPECT_EQ(interpolatingvector.get_allvalues(), controls);
}

TEST(InterpolatingVector, construct_from_json) {
  nlohmann::json values = R"(
{"id": "superid",
"data":
[
{
  "x": 0,
"values": [ 1,2,3]
},
{
  "x": 1,
"values": [ 10,20,30]
}
]
})"_json;
  std::vector<nlohmann::json> contains_x
      = {R"({"minimum": 0, "maximum": 0})"_json};

  auto schema = Aux::schema::make_initial_schema(2, 3, contains_x);

  auto interpolatingVector
      = InterpolatingVector::construct_from_json(values, schema);
  auto points = interpolatingVector.get_interpolation_points();
  std::vector<double> expected_points{0, 1};
  EXPECT_EQ(points, expected_points);

  Eigen::Vector3d expected_first{{1, 2, 3}};
  Eigen::Vector3d expected_second{{10, 20, 30}};

  Eigen::Vector3d first = interpolatingVector.mut_timestep(0);
  Eigen::Vector3d second = interpolatingVector.mut_timestep(1);

  EXPECT_EQ(first, expected_first);
  EXPECT_EQ(second, expected_second);

  for (Eigen::Index i = 0; i != 3; ++i) {
    EXPECT_DOUBLE_EQ(interpolatingVector(0)[i], expected_first[i]);
    EXPECT_DOUBLE_EQ(interpolatingVector(1)[i], expected_second[i]);
  }

  double lambda = 0.3;

  auto interpolated = interpolatingVector(lambda);
  auto expected_interpolated
      = (1 - lambda) * expected_first + lambda * expected_second;

  for (Eigen::Index i = 0; i != first.size(); ++i) {
    EXPECT_DOUBLE_EQ(interpolated[i], expected_interpolated[i]);
  }
}
