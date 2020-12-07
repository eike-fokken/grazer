#include <Isothermaleulerequation.hpp>
#include <cfloat>
#include <gtest/gtest.h>

Eigen::Vector2d p_qvol(3, 4);

// double rho = 0.5;
// double Re1 = 1000; //test for throwing exceptions
// double Re2 = 5000;
// double q = 0.4;

static constexpr double bar{1e5};
static constexpr double rho_0{0.785};
static constexpr double T_0{273.15};
static constexpr double T_crit{192.0};
static constexpr double z_0{1.005};
static constexpr double p_0{1.01325 * bar};
static constexpr double p_crit{46.4 * bar};
static constexpr double T{283.15};
static constexpr double alpha{(0.257 / p_crit - 0.533 * T_crit / (p_crit * T))};

static constexpr double eta{1e-5};

static constexpr double c_vac_squared{p_0 * T / (z_0 * T_0 * rho_0)};

TEST(testIsothermaleulerequation, flux) {

  double Area = 2.0;
  double diameter = 3.5;
  double roughness = 1.0;

  for (int i = 1; i != 21; ++i) {

    double rho = 10 * i;
    double q = 10 * i;

    Eigen::Vector2d state1(rho, q);

    Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, roughness);
    Eigen::Vector2d flux_vector = Iso.flux(state1);

    double f0 = rho_0 / Area * q;
    double f1 = Area / rho_0 * Iso.p(rho) + rho_0 / Area * q * q / rho;
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

    double rho = 10*i;
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

// TEST(testIsothermaleulerequation, source) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter,roughness);
// Eigen::Vector2d source_vector = Iso.source(state1);

// }

// TEST(testIsothermaleulerequation, dsource_dstate) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter,roughness);
// Eigen::Matrix2d dsource_matrix = Iso.dsource_dstate(state1);

// }

// TEST(testIsothermaleulerequation, p_qvol) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter,roughness);
// Eigen::Vector2d p_qvol_vector = Iso.p_qvol(state1);

// }

// TEST(testIsothermaleulerequation, dp_qvol_dstate) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter,roughness);
// Eigen::Matrix2d dp_qvol_dstate_vector = Iso.dp_qvol_dstate(state1);

// }

// TEST(testIsothermaleulerequation, state) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter,roughness);
// Eigen::Vector2d state_vector = Iso.state(p_qvol);

// }

// TEST(testIsothermaleulerequation, p) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter,roughness);
// double new_p = Iso.p(rho);

// }

// TEST(testIsothermaleulerequation, dp_drho) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter,roughness);
// double dp = Iso.dp_drho(rho);

// }

// TEST(testIsothermaleulerequation, rho) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter,roughness);
// double new_rho = Iso.rho(rho);

// }

// //For testing private memebers
// /*TEST(testIsothermaleulerequation, lambda_non_laminar) {

// Model::Balancelaw::Isothermaleulerequation Iso(Area, diameter, _eta,
// roughness); double dot_product = Iso.lambda_non_laminar(Re1); double
// dot_product = Iso.lambda_non_laminar(Re2);
// }

// TEST(testIsothermaleulerequation, dlambda_non_laminar_dRe) {

// Model::Balancelaw::Isothermaleulerequation(Area, diameter, _eta, roughness);

// }

// TEST(testIsothermaleulerequation, Reynolds) {

// }

// TEST(testIsothermaleulerequation, coeff_of_Reynolds) {

// }

// TEST(testIsothermaleulerequation, dReynolds_dq) {

// }

// TEST(testIsothermaleulerequation, Swamee_Jain) {

// }

// TEST(testIsothermaleulerequation, dSwamee_Jain_dRe) {

// }*/
