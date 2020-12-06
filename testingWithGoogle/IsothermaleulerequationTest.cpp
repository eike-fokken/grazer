#include <Isothermaleulerequation.hpp>
#include <gtest/gtest.h>


double _Area = 2.0;
double _diameter = 3.5;
double _roughness = 1.0;

Eigen::Vector2d state1(0,3);
Eigen::Vector2d p_qvol(3,4);

double rho = 0.5;
double Re1 = 1000; //test for throwing exceptions
double Re2 = 5000; 
double q = 0.4;

TEST(testIsothermaleulerequation, flux) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
Eigen::Vector2d flux_vector = Iso.flux(state1);

}

TEST(testIsothermaleulerequation, dflux_state) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
Eigen::Matrix2d dflux_matrix = Iso.dflux_dstate(state1);

}

TEST(testIsothermaleulerequation, source) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
Eigen::Vector2d source_vector = Iso.source(state1);

}

TEST(testIsothermaleulerequation, dsource_dstate) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
Eigen::Matrix2d dsource_matrix = Iso.dsource_dstate(state1);


}

TEST(testIsothermaleulerequation, p_qvol) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
Eigen::Vector2d p_qvol_vector = Iso.p_qvol(state1);

}

TEST(testIsothermaleulerequation, dp_qvol_dstate) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
Eigen::Matrix2d dp_qvol_dstate_vector = Iso.dp_qvol_dstate(state1);

}

TEST(testIsothermaleulerequation, state) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
Eigen::Vector2d state_vector = Iso.state(p_qvol);

}

TEST(testIsothermaleulerequation, p) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
double new_p = Iso.p(rho);

}

TEST(testIsothermaleulerequation, dp_drho) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
double dp = Iso.dp_drho(rho);

}

TEST(testIsothermaleulerequation, rho) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter,_roughness);
double new_rho = Iso.rho(rho);

}

//For testing private memebers
/*TEST(testIsothermaleulerequation, lambda_non_laminar) {

Model::Balancelaw::Isothermaleulerequation Iso(_Area, _diameter, _eta, _roughness);
double dot_product = Iso.lambda_non_laminar(Re1);
double dot_product = Iso.lambda_non_laminar(Re2);
}

TEST(testIsothermaleulerequation, dlambda_non_laminar_dRe) {

Model::Balancelaw::Isothermaleulerequation(_Area, _diameter, _eta, _roughness);


}

TEST(testIsothermaleulerequation, Reynolds) {


}

TEST(testIsothermaleulerequation, coeff_of_Reynolds) {


}

TEST(testIsothermaleulerequation, dReynolds_dq) {


}

TEST(testIsothermaleulerequation, Swamee_Jain) {


}

TEST(testIsothermaleulerequation, dSwamee_Jain_dRe) {


}*/






