#pragma once
#include "Pipe_Balancelaw.hpp"
#include <Eigen/Dense>

namespace Model::Balancelaw {

  class Isothermaleulerequation : public Pipe_Balancelaw {

  public:
    Isothermaleulerequation(){};

    Eigen::Vector2d flux(
        Eigen::Ref<Eigen::Vector2d const> state,
        double diameter) const override;
    Eigen::Matrix2d dflux_dstate(
        Eigen::Ref<Eigen::Vector2d const> state,
        double diameter) const override;

    Eigen::Vector2d source(
        Eigen::Ref<Eigen::Vector2d const> state, double diameter,
        double roughness) const override;
    Eigen::Matrix2d dsource_dstate(
        Eigen::Ref<Eigen::Vector2d const> state, double diameter,
        double roughness) const override;

    Eigen::Vector2d
    p_qvol(Eigen::Ref<Eigen::Vector2d const> state) const override;
    Eigen::Matrix2d
    dp_qvol_dstate(Eigen::Ref<Eigen::Vector2d const> state) const override;

    Eigen::Vector2d
    state(Eigen::Ref<Eigen::Vector2d const> p_qvol) const override;
    double p_pascal_from_p_bar(double p) const override;
    Eigen::Vector2d p_qvol_from_p_qvol_bar(
        Eigen::Ref<Eigen::Vector2d const> p_qvol_bar) const override;

    double p_bar_from_p_pascal(double p) const override;
    double dp_bar_from_p_pascal_dp_pascal(double p) const override;

    Eigen::Vector2d p_qvol_bar_from_p_qvol(
        Eigen::Ref<Eigen::Vector2d const> p_qvol) const override;
    Eigen::Matrix2d dp_qvol_bar_from_p_qvold_p_qvol(
        Eigen::Ref<Eigen::Vector2d const> p_qvol) const override;

    double p(double rho) const override;
    double dp_drho(double rho) const override;
    double rho(double rho) const override;

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
  };
} // namespace Model::Balancelaw
