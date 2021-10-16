#include "Controlhelpers.hpp"

#include "Exception.hpp"
#include <gmock/gmock-matchers.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using namespace Aux;

TEST(Vector_interpolator, Construction_happy_path) {

  int number_of_controls_per_timestep = 4;
  int number_of_timesteps = 8;

  Vector_interpolator controller(number_of_controls_per_timestep, number_of_timesteps);

  EXPECT_EQ(
      controller.get_number_of_controls(),
      number_of_controls_per_timestep * number_of_timesteps);
}

TEST(Vector_interpolator, Construction_negative_number_of_controls) {

  int number_of_controls_per_timestep = -1;
  int number_of_timesteps = 8;

  try {
    Vector_interpolator controller(number_of_controls_per_timestep, number_of_timesteps);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr(
                      "You can't have less than 0 controls per time step!"));
  }
}

TEST(Vector_interpolator, Construction_nonpositive_number_steps) {

  int number_of_controls_per_timestep = 4;
  int number_of_timesteps = 0;

  try {
    Vector_interpolator controller(number_of_controls_per_timestep, number_of_timesteps);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr("You can't have less than 1 time step!"));
  }
}

TEST(Vector_interpolator, set_and_evaluate_controls_happy_path) {

  int number_of_controls_per_timestep = 4;
  int number_of_timesteps = 8;

  Vector_interpolator controller(number_of_controls_per_timestep, number_of_timesteps);

  Eigen::VectorXd controls(controller.get_number_of_controls());

  for (int index = 0; index != controls.size(); ++index) {
    controls[index] = index;
  }
  controller.set_controls(controls);

  for (int i = 0; i != number_of_timesteps; ++i) {
    EXPECT_EQ(
        controller(i), controls.segment(
                           i * number_of_controls_per_timestep,
                           number_of_controls_per_timestep));
  }
  EXPECT_EQ(
      controls[controls.size() - 1],
      controller(number_of_timesteps - 1)[number_of_controls_per_timestep - 1]);
}

TEST(Vector_interpolator, set_controls_invalid) {
  int number_of_controls_per_timestep = 4;
  int number_of_timesteps = 8;

  Vector_interpolator controller(number_of_controls_per_timestep, number_of_timesteps);

  Eigen::VectorXd controls(controller.get_number_of_controls() - 1);

  try {
    controller.set_controls(controls);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr(
            "You are trying to assign the wrong number of controls."));
  }
}

TEST(Vector_interpolator, evaluate_controls_invalid_high_index) {

  int number_of_controls_per_timestep = 8;
  int number_of_timesteps = 88;

  Vector_interpolator controller(number_of_controls_per_timestep, number_of_timesteps);

  Eigen::VectorXd controls(controller.get_number_of_controls());

  for (int index = 0; index != controls.size(); ++index) {
    controls[index] = index;
  }
  controller.set_controls(controls);

  try {
    controller(number_of_controls_per_timestep * number_of_timesteps);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(), testing::HasSubstr(
                      "You request controls at a higher index than possible!"));
  }
}

TEST(Vector_interpolator, evaluate_controls_invalid_low_index) {

  int number_of_controls_per_timestep = 8;
  int number_of_timesteps = 88;

  Vector_interpolator controller(number_of_controls_per_timestep, number_of_timesteps);

  Eigen::VectorXd controls(controller.get_number_of_controls());

  for (int index = 0; index != controls.size(); ++index) {
    controls[index] = index;
  }
  controller.set_controls(controls);

  try {
    controller(-1);
    FAIL() << "Test FAILED: The statement ABOVE\n"
           << __FILE__ << ":" << __LINE__ << "\nshould have thrown!";
  } catch (std::runtime_error &e) {
    EXPECT_THAT(
        e.what(),
        testing::HasSubstr("You request controls at negative time steps!"));
  }
}

TEST(Vector_interpolator, mut_timestep_happy_path) {

  int number_of_controls_per_timestep = 4;
  int number_of_timesteps = 8;

  Vector_interpolator controller(number_of_controls_per_timestep, number_of_timesteps);

  Eigen::VectorXd controls(controller.get_number_of_controls());

  for (int index = 0; index != controls.size(); ++index) {
    controls[index] = index;
  }
  controller.set_controls(controls);

  for (int i = 0; i != number_of_timesteps; ++i) {
    controller.mut_timestep(i) = controls.segment(
        i * number_of_controls_per_timestep, number_of_controls_per_timestep);
  }
  EXPECT_EQ(controller.get_allcontrols(), controls);
}
