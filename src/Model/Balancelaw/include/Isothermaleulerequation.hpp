#pragma once
#include <Eigen/Dense>



namespace Model::Balancelaw {

class Isothermaleulerequation {

public:

  // is this needed? Maybe not because we use c++17...
  //  EIGEN_MAKE_ALIGNED_OPERATOR_NEW


  Isothermaleulerequation(double _Area, double _diameter,
                          double _roughness);

  Eigen::Vector2d flux(Eigen::Vector2d const &state) const;
  Eigen::Matrix2d dflux_dstate(Eigen::Vector2d const &state) const;

  Eigen::Vector2d source(Eigen::Vector2d const & state) const;
  Eigen::Matrix2d dsource_dstate(Eigen::Vector2d const & state) const;

  Eigen::Vector2d p_qvol(Eigen::Vector2d const & state) const;
  Eigen::Matrix2d dp_qvol_dstate(Eigen::Vector2d const &state) const;

  Eigen::Vector2d state(Eigen::Vector2d const & state) const;

  double p(double p) const;
  double dp_drho(double rho) const;
  double rho(double rho) const;

private:
  double lambda_non_laminar(double Re) const;
  double dlambda_non_laminar_dRe(double Re) const;
  double Reynolds(double q) const;
  double coeff_of_Reynolds() const;
  double dReynolds_dq(double q) const;

  double Swamee_Jain(double Re) const;
  double dSwamee_Jain_dRe(double Re) const;

  // These are physical constants except for the temperature T.
  // Later on it may be useful to change them to include other gases.
  static constexpr double bar { 1e5};
  static constexpr double rho_0 { 0.785};
  static constexpr double T_0 { 273.15};
  static constexpr double T_crit { 192.0};
  static constexpr double z_0 { 1.005};
  static constexpr double p_0 { 1.01325 * bar};
  static constexpr double p_crit { 46.4 * bar};
  static constexpr double T { 283.15};
  static constexpr double alpha { (0.257 / p_crit - 0.533 * T_crit / (p_crit * T))};

  static constexpr double eta { 1e-5};

  static constexpr double c_vac_squared { p_0 * T/(z_0*T_0*rho_0)};


  double const Area;
  double const diameter;
  double const roughness;

  Eigen::Vector4d coefficients;

  // /// This is a tolerance for the newton iteration:
  // static constexpr double tolerance = 1e-6;
  // /// This is an ad-hoc mechanism to prevent too many Newton iterations in finding the correct friction factor lambda.
  // double last_mu{10.0};
  

  };

}
