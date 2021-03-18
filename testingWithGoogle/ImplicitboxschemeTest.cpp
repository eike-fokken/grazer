#include <Isothermaleulerequation.hpp>
#include <Implicitboxscheme.hpp>
#include <gtest/gtest.h>



TEST(testImplicitboxscheme, evaluate_point) {

  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation bl;
  
  
  Eigen::VectorXd new_state(4);
  Eigen::VectorXd last_state(4);



  new_state << 63, -90, 60, 50;
  last_state << 73, 68, 73, 55;

  double last_time = 0;
  double new_time = 10;
  double Delta_x = 15;

  double Delta_t = new_time-last_time;
  Eigen::Vector2d last_left = last_state.segment<2>(0);
  Eigen::Vector2d last_right = last_state.segment<2>(2);

  Eigen::Vector2d new_left = new_state.segment<2>(0);
  Eigen::Vector2d new_right = new_state.segment<2>(2);

  Eigen::Vector2d flux_right = bl.flux(new_right,diameter);
  Eigen::Vector2d flux_left = bl.flux(new_left,diameter);

  Eigen::Vector2d source_right = bl.source(new_right,diameter,roughness);
  Eigen::Vector2d source_left = bl.source(new_left,diameter,roughness);


  Eigen::Vector2d expected_result = 0.5*(new_left +new_right) - 0.5*(last_left+last_right) + Delta_t/Delta_x*(flux_right -flux_left) - 0.5*Delta_t* (source_left+source_right);

  Eigen::Vector2d result;

  Model::Scheme::Implicitboxscheme scheme;
  scheme.evaluate_point(result, last_time, new_time, Delta_x, last_left,
                        last_right, new_left, new_right, bl, diameter,
                        roughness);

  EXPECT_DOUBLE_EQ(result[0],expected_result[0]);
  EXPECT_DOUBLE_EQ(result[1],expected_result[1]);

}


TEST(testImplicitboxscheme, devaluate_point_dleft) {

  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation bl;

  
  Eigen::VectorXd new_state(4);
  Eigen::VectorXd last_state(4);

  double epsilon = pow(DBL_EPSILON,1.0/3.0);

  double finite_difference_threshold = sqrt(epsilon);

    Eigen::Vector2d h0(epsilon,0);
    Eigen::Vector2d h1(0,epsilon);
    
    new_state << 63, -90, 60, 50;
    last_state << 73, 68, 73, 55;

  double last_time = 0;
  double new_time = 10;
  double Delta_x = 15;

  Eigen::Vector2d last_left = last_state.segment<2>(0);
  Eigen::Vector2d last_right = last_state.segment<2>(2);

  Eigen::Vector2d new_left = new_state.segment<2>(0);
  Eigen::Vector2d new_right = new_state.segment<2>(2);

  Model::Scheme::Implicitboxscheme scheme;

  Eigen::Vector2d result_h0;
  Eigen::Vector2d result_mh0;

  scheme.evaluate_point(result_h0, last_time, new_time, Delta_x, last_left,
                        last_right, new_left + h0, new_right, bl, diameter,
                        roughness);

  scheme.evaluate_point(result_mh0, last_time, new_time, Delta_x, last_left,
                        last_right, new_left - h0, new_right, bl, diameter,
                        roughness);

  Eigen::Vector2d difference_derivative0 =0.5*( result_h0-result_mh0)/epsilon;

  Eigen::Matrix2d jacobimatrix = scheme.devaluate_point_dleft(
      last_time, new_time, Delta_x, last_left, last_right, new_left, new_right,
      bl, diameter, roughness);
  Eigen::Vector2d analytical_derivative = jacobimatrix*h0/epsilon;

  EXPECT_NEAR(analytical_derivative[0],difference_derivative0[0],finite_difference_threshold);
  EXPECT_NEAR(analytical_derivative[1],difference_derivative0[1],finite_difference_threshold);
  Eigen::Vector2d result_h1;
  Eigen::Vector2d result_mh1;
  scheme.evaluate_point(result_h1, last_time, new_time, Delta_x, last_left,
                        last_right, new_left + h1, new_right, bl, diameter,
                        roughness);

  scheme.evaluate_point(result_mh1, last_time, new_time, Delta_x, last_left,
                        last_right, new_left - h1, new_right, bl, diameter,
                        roughness);

  Eigen::Vector2d difference_derivative1 =0.5*( result_h1-result_mh1)/epsilon;

  Eigen::Matrix2d jacobimatrix1 = scheme.devaluate_point_dleft(
      last_time, new_time, Delta_x, last_left, last_right, new_left, new_right,
      bl, diameter, roughness);
  Eigen::Vector2d analytical_derivative1 = jacobimatrix1*h1/epsilon;

  EXPECT_NEAR(analytical_derivative1[0],difference_derivative1[0],finite_difference_threshold);
  EXPECT_NEAR(analytical_derivative1[1],difference_derivative1[1],finite_difference_threshold);
  

}


TEST(testImplicitboxscheme, devaluate_point_dright) {

  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation bl;

  
  Eigen::VectorXd new_state(4);
  Eigen::VectorXd last_state(4);

  double epsilon = pow(DBL_EPSILON,1.0/3.0);

  double finite_difference_threshold = sqrt(epsilon);

    Eigen::Vector2d h0(epsilon,0);
    Eigen::Vector2d h1(0,epsilon);
    
    new_state << 63, -90, 60, 50;
    last_state << 73, 68, 73, 55;

  double last_time = 0;
  double new_time = 10;
  double Delta_x = 15;

  Eigen::Vector2d last_left = last_state.segment<2>(0);
  Eigen::Vector2d last_right = last_state.segment<2>(2);

  Eigen::Vector2d new_left = new_state.segment<2>(0);
  Eigen::Vector2d new_right = new_state.segment<2>(2);

  Model::Scheme::Implicitboxscheme scheme;

  Eigen::Vector2d result_h0;
  Eigen::Vector2d result_mh0;

  scheme.evaluate_point(result_h0, last_time, new_time, Delta_x, last_left,
                        last_right, new_left, new_right + h0, bl, diameter,
                        roughness);

  scheme.evaluate_point(result_mh0, last_time, new_time, Delta_x, last_left,
                        last_right, new_left, new_right - h0, bl, diameter,
                        roughness);

  Eigen::Vector2d difference_derivative0 =0.5*( result_h0-result_mh0)/epsilon;

  Eigen::Matrix2d jacobimatrix = scheme.devaluate_point_dright(
      last_time, new_time, Delta_x, last_left, last_right, new_left, new_right,
      bl, diameter, roughness);
  Eigen::Vector2d analytical_derivative = jacobimatrix*h0/epsilon;

  EXPECT_NEAR(analytical_derivative[0],difference_derivative0[0],finite_difference_threshold);
  EXPECT_NEAR(analytical_derivative[1],difference_derivative0[1],finite_difference_threshold);


  Eigen::Vector2d result_h1;
  Eigen::Vector2d result_mh1;
  scheme.evaluate_point(result_h1, last_time, new_time, Delta_x, last_left,
                        last_right, new_left, new_right + h1, bl, diameter,
                        roughness);

  scheme.evaluate_point(result_mh1, last_time, new_time, Delta_x, last_left,
                        last_right, new_left, new_right - h1, bl, diameter,
                        roughness);

  Eigen::Vector2d difference_derivative1 = 0.5*( result_h1-result_mh1)/epsilon;

  Eigen::Matrix2d jacobimatrix1 = scheme.devaluate_point_dright(
      last_time, new_time, Delta_x, last_left, last_right, new_left, new_right,
      bl, diameter, roughness);
  Eigen::Vector2d analytical_derivative1 = jacobimatrix1*h1/epsilon;

  EXPECT_NEAR(analytical_derivative1[0],difference_derivative1[0],finite_difference_threshold);
  EXPECT_NEAR(analytical_derivative1[1],difference_derivative1[1],finite_difference_threshold);
  

}
