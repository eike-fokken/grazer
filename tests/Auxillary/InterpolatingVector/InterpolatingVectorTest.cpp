#include "InterpolatingVector.hpp"

#include "Exception.hpp"
#include "make_schema.hpp"
#include <Eigen/Sparse>
#include <cstddef>
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Aux;

// Used to actually call the base assignment.
static void assign(InterpolatingVector_Base &a, InterpolatingVector_Base &b) {
  a = b;
}

TEST(InterpolatingVector, Copy_constructor_happy) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);

  Aux::InterpolatingVector b(data, number_of_values_per_point);

  Aux::InterpolatingVector a = b;

  Aux::InterpolatingVector_Base &bbase = b;

  Aux::InterpolatingVector c = bbase;
  EXPECT_EQ(a, b);
  EXPECT_EQ(c, b);
}

TEST(InterpolatingVector, Construction_happy_path) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);
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
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);

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
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);
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
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);
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

  Eigen::Vector3d first = interpolatingVector.vector_at_index(0);
  Eigen::Vector3d second = interpolatingVector.vector_at_index(1);

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

TEST(InterpolatingVector, equality_happypath) {
  auto data = Aux::make_from_start_delta_end(0, 1, 1);

  Aux::InterpolatingVector a(data, 2);
  Aux::InterpolatingVector b(data, 2);
  Eigen::VectorXd values{{1, 2, 3, 4}};
  a.set_values_in_bulk(values);
  b.set_values_in_bulk(values);

  EXPECT_EQ(a, b);
}

TEST(InterpolatingVector, equality_different_interpolating_points) {
  auto adata = Aux::make_from_start_delta_end(0, 1, 1);
  auto bdata = Aux::make_from_start_delta_end(0, 1, 4);

  Aux::InterpolatingVector a(adata, 2);
  Aux::InterpolatingVector b(bdata, 2);
  Eigen::VectorXd values{{1, 2, 3, 4}};
  a.set_values_in_bulk(values);

  EXPECT_NE(a, b);
}

TEST(InterpolatingVector, equality_different_inner_lengths) {
  auto adata = Aux::make_from_start_delta_end(0, 1, 1);

  Aux::InterpolatingVector a(adata, 2);
  Aux::InterpolatingVector b(adata, 3);
  Eigen::VectorXd avalues{{1, 2, 3, 4}};
  Eigen::VectorXd bvalues{{1, 2, 3, 4, 5, 6}};
  a.set_values_in_bulk(avalues);
  b.set_values_in_bulk(bvalues);

  EXPECT_NE(a, b);
}

TEST(MappedInterpolatingVector, Copy_assignment_happy) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);
  Aux::InterpolatingVector b(data, number_of_values_per_point);

  std::array<double, 32> x;
  Aux::MappedInterpolatingVector c(
      data, number_of_values_per_point, x.data(), x.size());
  c = b;

  EXPECT_EQ(c, b);
}

TEST(MappedInterpolatingVector, Copy_assignment_fail) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);
  Aux::InterpolatingVector b(data, number_of_values_per_point);

  std::array<double, 24> x;
  Aux::MappedInterpolatingVector c(data, 3, x.data(), x.size());

  try {
    c = b;
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr(
                      " You are trying to assign an InterpolatingVector with a "
                      "different structure to this InterpolatingVector."));
  }
}

TEST(MappedInterpolatingVector, Constructor_fail) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);
  Aux::InterpolatingVector b(data, number_of_values_per_point);

  std::array<double, 32> x;
  try {
    Aux::MappedInterpolatingVector c(
        data, number_of_values_per_point, x.data(), x.size() - 1);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr(
            "Given number of elements of mapped storage differs from needed "));
  }
}

TEST(InterpolatingVector_Base, assignment_Normal_happy) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);
  Aux::InterpolatingVector a(data, number_of_values_per_point);
  Aux::InterpolatingVector b(data, number_of_values_per_point);

  assign(a, b);

  EXPECT_EQ(a, b);
}

TEST(InterpolatingVector_Base, assignment_Mapped_happy) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);
  Aux::InterpolatingVector b(data, number_of_values_per_point);

  std::array<double, 32> x;
  Aux::MappedInterpolatingVector c(
      data, number_of_values_per_point, x.data(), x.size());

  assign(c, b);

  EXPECT_EQ(c, b);
}

TEST(InterpolatingVector_Base, assignment_Mapped_fail) {

  int number_of_values_per_point = 4;
  int number_of_points = 8;
  double start = 0;
  double delta = 0.5;
  auto data = Aux::make_from_start_delta_number(start, delta, number_of_points);
  Aux::InterpolatingVector b(data, number_of_values_per_point);

  std::array<double, 24> x;
  Aux::MappedInterpolatingVector c(data, 3, x.data(), x.size());

  try {
    assign(c, b);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("This is not permitted through the "
                                     "InterpolatingVector_Base interface."));
  }
}
