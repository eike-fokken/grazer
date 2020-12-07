#include <Isothermaleulerequation.hpp>
#include <cfloat>
#include <gtest/gtest.h>



TEST(testIsothermaleulerequation, flux) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);
  for (int i = 1; i != 21; ++i) {

    double rho =  i;
    double q = 10 * i;

    Eigen::Vector2d state1(rho, q);

    Eigen::Vector2d flux_vector = Iso.flux(state1);

    double f0 = Iso.rho_0 / Area * q;
    double f1 = Area / Iso.rho_0 * Iso.p(rho) + Iso.rho_0 / Area * q * q / rho;
    EXPECT_DOUBLE_EQ(f0, flux_vector[0]);
    EXPECT_DOUBLE_EQ(f1, flux_vector[1]);

  }
  }

TEST(testIsothermaleulerequation, dflux_state) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);

  for (int i = 1; i != 21; ++i) {

    double rho = i;
    double q = 10 * i;

    Eigen::Vector2d x(rho, q);
    double epsilon = pow(DBL_EPSILON,1.0/3.0);

    double finite_difference_threshold = sqrt(epsilon);

    Eigen::Vector2d h0(epsilon, 0);
    

    Eigen::Matrix2d exact_dflux_matrix = Iso.dflux_dstate(x);

    Eigen::Vector2d difference_dflux0 = 0.5*(Iso.flux(x + h0) - Iso.flux(x-h0));
    Eigen::Vector2d exact_dflux0 = exact_dflux_matrix * h0;

    EXPECT_NEAR(exact_dflux0[0], difference_dflux0[0],
                finite_difference_threshold);
    EXPECT_NEAR(exact_dflux0[1], difference_dflux0[1],
                finite_difference_threshold);

    Eigen::Vector2d h1(0, epsilon);

    Eigen::Vector2d difference_dflux1 = 0.5 * (Iso.flux(x + h1) - Iso.flux(x-h1));
    Eigen::Vector2d exact_dflux1 = exact_dflux_matrix * h1;

    EXPECT_NEAR(exact_dflux1[0], difference_dflux1[0],
                finite_difference_threshold);
    EXPECT_NEAR(exact_dflux1[1], difference_dflux1[1],
                finite_difference_threshold);
  }
}

TEST(testIsothermaleulerequation, source) {
  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);
  for (int i = 1; i != 21; ++i) {

    double rho = i;
    double q = 10 * (i-10)+9e-7;

    Eigen::Vector2d state1(rho, q);

    Eigen::Vector2d source_vector = Iso.source(state1);


    if (std::abs(q) > 1e-6) {
      EXPECT_TRUE(source_vector[1] * q < 0);
    } else {
      // This test is probably very dependent on Area so be careful with it...
      EXPECT_TRUE(std::abs(source_vector[1] * q) < 1e-9);
    }
  }
  }

// TEST(testIsothermaleulerequation, dsource_dstate) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter,roughness);
// Eigen::Matrix2d dsource_matrix = Iso.dsource_dstate(state1);

// }

TEST(testIsothermaleulerequation, p_qvol_and_state) {

double Area = 2.0;
double diameter = 3.5;
double roughness = 1.0;

 Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);
for (int i = 1; i != 21; ++i) {

  double rho = i;
  double q = 10 * i;

  Eigen::Vector2d state1(rho, q);
  Eigen::Vector2d p_qvol_vector = Iso.p_qvol(state1);

  Eigen::Vector2d state2 = Iso.state(p_qvol_vector);

  EXPECT_DOUBLE_EQ(state1[0], state2[0]);
  EXPECT_DOUBLE_EQ(state1[1], state2[1]);
 }
}

TEST(testIsothermaleulerequation, dp_qvol_dstate) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;

  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);

  for (int i = 1; i != 21; ++i) {

    double rho = i;
    double q = 10 * i;

    Eigen::Vector2d x(rho, q);
    double epsilon = pow(DBL_EPSILON,1.0/3.0);

    double finite_difference_threshold = sqrt(epsilon);

    Eigen::Vector2d h0(epsilon, 0);
    
    Eigen::Matrix2d exact_dp_qvol_dstate_matrix = Iso.dp_qvol_dstate(x);

    Eigen::Vector2d difference_dp_qvol0 = 0.5*(Iso.p_qvol(x + h0) - Iso.p_qvol(x-h0));
    Eigen::Vector2d exact_dflux0 = exact_dp_qvol_dstate_matrix * h0;

    EXPECT_NEAR(exact_dflux0[0], difference_dp_qvol0[0],
                finite_difference_threshold);
    EXPECT_NEAR(exact_dflux0[1], difference_dp_qvol0[1],
                finite_difference_threshold);

    Eigen::Vector2d h1(0, epsilon);

    Eigen::Vector2d difference_dp_qvol1 = 0.5 * (Iso.p_qvol(x + h1) - Iso.p_qvol(x-h1));
    Eigen::Vector2d exact_dflux1 = exact_dp_qvol_dstate_matrix * h1;

    EXPECT_NEAR(exact_dflux1[0], difference_dp_qvol1[0],
                finite_difference_threshold);
    EXPECT_NEAR(exact_dflux1[1], difference_dp_qvol1[1],
                finite_difference_threshold);
  }
}


TEST(testIsothermaleulerequation, p_and_rho) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);
  for (int i = 1; i != 21; ++i) {

    double rho = i;
    double p = 100000 * i;
    
    double computed_rho= Iso.rho(Iso.p(rho));
    double computed_p = Iso.p(Iso.rho(p));


    EXPECT_DOUBLE_EQ(rho, computed_rho);
    EXPECT_DOUBLE_EQ(p, computed_p);
  }

  }

TEST(testIsothermaleulerequation, dp_drho) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);


  EXPECT_DOUBLE_EQ(Iso.c_vac_squared, Iso.dp_drho(0.0));
  for (int i = 1; i != 21; ++i) {

    double rho = i;
    double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
    double h = epsilon;
    double finite_difference_threshold = sqrt(epsilon);
    double exact_dp = Iso.dp_drho(rho);
    double difference_dp = 0.5 * (Iso.p(rho + h) - Iso.p(rho -h))/h;
    EXPECT_NEAR(exact_dp, difference_dp,
                finite_difference_threshold);
  }
  }


TEST(testIsothermaleulerequation, lambda_non_laminar) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);

  {
  double Re=2000-1e-9;
  double lambda = Iso.lambda_non_laminar(2000);
  double expected_lambda = 64/Re;
  EXPECT_NEAR(lambda, expected_lambda, 1e-9);
 }
 {
   double Re = 4000-1e-9;
   double lambda = Iso.lambda_non_laminar(Re);
   double expected_lambda = Iso.Swamee_Jain(4000);
   EXPECT_NEAR(lambda, expected_lambda,1e-9);
 }
}
TEST(testIsothermaleulerequation, dlambda_non_laminar_dRe) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;
  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);

  {
    double Re0 = 2000+1e-9;
    double dlambda = Iso.dlambda_non_laminar_dRe(Re0);
    double expected_dlambda = -64./(2000*2000);
    EXPECT_NEAR(dlambda, expected_dlambda, 1e-9);
  }
  {
    double Re1 = 4000 - 1e-9;
    double dlambda = Iso.dlambda_non_laminar_dRe(Re1);
    double expected_dlambda = Iso.dSwamee_Jain_dRe(4000);
    EXPECT_NEAR(dlambda, expected_dlambda, 1e-9);
  }
  for(int i=1;i!=20;++i){

    double Re = 2000+ i * 100;
    double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
    double h = epsilon;
    double finite_difference_threshold = sqrt(epsilon);
    double exact_dlambda = Iso.dlambda_non_laminar_dRe(Re);
    double difference_dlambda =
        0.5 * (Iso.lambda_non_laminar(Re + h) - Iso.lambda_non_laminar(Re - h)) / h;
    EXPECT_NEAR(exact_dlambda, difference_dlambda,
                finite_difference_threshold);
  }


}

// TEST(testIsothermaleulerequation, Reynolds) {

// }

TEST(testIsothermaleulerequation, coeff_of_Reynolds) {


}

TEST(testIsothermaleulerequation, dReynolds_dq) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;

  double q0 = 5e-3;
  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);

  EXPECT_ANY_THROW(Iso.dReynolds_dq(q0));

  for (int i = 1; i != 21; ++i) {
    double q = 10 * (i-10);
    double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
    double h = epsilon;
    double finite_difference_threshold = sqrt(epsilon);
  
    if (Iso.Reynolds(q) > 2000) {
      double exact_dReynolds = Iso.dReynolds_dq(q);
      double difference_dReynolds =
          0.5 * (Iso.Reynolds(q + h) - Iso.Reynolds(q - h)) / h;
      EXPECT_NEAR(exact_dReynolds, difference_dReynolds,
                  finite_difference_threshold);
    }
  }
}
TEST(testIsothermaleulerequation, Swamee_Jain) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;

  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);

  double Re0 = 3999;

  EXPECT_ANY_THROW(Iso.Swamee_Jain(Re0));

  for (int i = 4; i != 10; ++i) {
    double Re = i*1000;

    double lambda = Iso.Swamee_Jain( Re);

    double lambda_exact = Iso.exact_turbulent_lambda( Re);
      double test = (abs(lambda - lambda_exact))/lambda_exact;
      EXPECT_NEAR(test, 0.0,3e-2);
  }


}

TEST(testIsothermaleulerequation, dSwamee_Jain_dRe) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;

  Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);

  double Re0 = 3999;

  EXPECT_ANY_THROW(Iso.Swamee_Jain(Re0));

  for (int i = 5; i != 10; ++i) {
    double Re = i * 1000;
    double epsilon = pow(DBL_EPSILON, 1.0 / 3.0);
    double h = epsilon;
    double finite_difference_threshold = sqrt(epsilon);
    double exact_dSwamee_Jain_dRe = Iso.dSwamee_Jain_dRe(Re);
    double difference_dSwamee_Jain_dRe = 0.5 * (Iso.Swamee_Jain(Re + h) - Iso.Swamee_Jain(Re - h)) / h;
    EXPECT_NEAR(exact_dSwamee_Jain_dRe, difference_dSwamee_Jain_dRe, finite_difference_threshold);
  }
}

