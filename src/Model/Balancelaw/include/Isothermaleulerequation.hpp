#pragma once
#include <Eigen/Dense>

namespace Model::Balancelaw {

  class Isothermaleulerequation {

  public:
    Isothermaleulerequation(){};

    static Eigen::Vector2d flux(Eigen::Vector2d const &state, double diameter);
    static Eigen::Matrix2d
    dflux_dstate(Eigen::Vector2d const &state, double diameter);

    static Eigen::Vector2d
    source(Eigen::Vector2d const &state, double diameter, double roughness);
    static Eigen::Matrix2d dsource_dstate(
        Eigen::Vector2d const &state, double diameter, double roughness);

    static Eigen::Vector2d p_qvol(Eigen::Vector2d const &state);
    static Eigen::Matrix2d dp_qvol_dstate(Eigen::Vector2d const &state);

    static Eigen::Vector2d state(Eigen::Vector2d const &p_qvol);
    static double p_pascal_from_p_bar(double p);
    static Eigen::Vector2d
    p_qvol_from_p_qvol_bar(Eigen::Vector2d const &p_qvol_bar);

    static double p_bar_from_p_pascal(double p);
    static double dp_bar_from_p_pascal_dp_pascal(double p);

    static Eigen::Vector2d
    p_qvol_bar_from_p_qvol(Eigen::Vector2d const &p_qvol);
    static Eigen::Matrix2d
    dp_qvol_bar_from_p_qvold_p_qvol(Eigen::Vector2d const &p_qvol);

    static double p(double rho);
    static double dp_drho(double rho);
    static double rho(double rho);

    static double
    lambda_non_laminar(double Re, double diameter, double roughness);
    static double
    dlambda_non_laminar_dRe(double Re, double diameter, double roughness);
    static double Reynolds(double q, double diameter);
    static double coeff_of_Reynolds(double diameter);
    static double dReynolds_dq(double q, double diameter);

    static double Swamee_Jain(double Re, double diameter, double roughness);
    static double
    dSwamee_Jain_dRe(double Re, double diameter, double roughness);

    static double
    exact_turbulent_lambda(double Re, double diameter, double roughness);

    // These are physical constants except for the temperature T.
    // Later on it may be useful to change them to include other gases.
    static constexpr double bar{1e5};
    static constexpr double rho_0{0.785};
    static constexpr double T_0{273.15};
    static constexpr double T_crit{192.033};
    static constexpr double z_0{1.005};
    static constexpr double p_0{1.01325 * bar};
    static constexpr double p_crit{46.4512 * bar};
    static constexpr double T{283.15};
    static constexpr double alpha{
        (0.257 / p_crit - 0.533 * T_crit / (p_crit * T))};

    static constexpr double eta{1e-5};

    static constexpr double c_vac_squared{p_0 * T / (z_0 * T_0 * rho_0)};

  private:
    static Eigen::Vector4d get_coefficients(double diameter, double roughness);

    static constexpr double laminar_border{2000.0};
    static constexpr double turbulent_border{4000.0};

    static Eigen::Matrix4d const set_coeff_helper_matrix();

    static const Eigen::Matrix4d coeff_helper_matrix;

    // Eigen::Vector4d coefficients;

    // /// This is a tolerance for the newton iteration:
    // static constexpr double tolerance = 1e-6;
    // /// This is an ad-hoc mechanism to prevent too many Newton iterations in
    // finding the correct friction factor lambda. double last_mu{10.0};
  };
} // namespace Model::Balancelaw
