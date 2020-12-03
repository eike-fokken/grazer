#include <Exception.hpp>
#include <Isothermaleulerequation.hpp>
#include <Mathfunctions.hpp>
#include <math.h>
#include <string>

namespace Model::Balancelaw {

  Isothermaleulerequation::Isothermaleulerequation(double _Area,
                                                   double _diameter,
                                                   double _eta,
                                                   double _roughness)
    : Area(_Area), diameter(_diameter), eta(_eta),roughness(_roughness) {
    double a = 2000;
    double b = 4000;

    Eigen::Matrix4d aux;
    aux << a * a * a, a * a, a, 1, //
        3 * a * a, 2 * a, 1, 0,  //
        b * b * b, b * b, b, 1,  //
        3 * b * b, 2 * b, 1, 0;  //
    Eigen::Vector4d spline_constraints(2000, 64. / 2000.0, 4000, Swamee_Jain(4000));
    coefficients = aux.lu().solve(spline_constraints);
  }

  Eigen::Vector2d
  Isothermaleulerequation::flux(Eigen::Vector2d const &state) const {

    double rho = state[0];
    double q = state[1];

    Eigen::Vector2d flux;

    flux[0] = rho_0 / Area * q;
    flux[1] = Area / rho_0 * p(rho) + rho_0 / Area * q * q / rho;
    return flux;
  }

  Eigen::Matrix2d
  Isothermaleulerequation::derivative_flux(Eigen::Vector2d const &state) const {

    double rho = state[0];
    double q = state[1];

    Eigen::Matrix2d dflux;

    dflux(0, 0) = 0.0;
    dflux(0, 1) = rho_0 / Area;
    dflux(1, 0) =
        Area / rho_0 * derivative_p(rho) - rho_0 / Area * q * q / (rho * rho);
    dflux(1, 1) = 2 * rho_0 / Area * q / rho;
    return dflux;
  }

  Eigen::Vector2d
  Isothermaleulerequation::sourceterm(Eigen::Vector2d const &state) {

    double rho = state[0];
    double q = state[1];

    Eigen::Vector2d source;
    source[0] = 0.0;
    if(Reynolds(q)<2000){
      source[1] =
          -rho_0 / (2 * Area* diameter) * 64 / coeff_of_Reynolds() * q / rho;
    } else {
      source[1] = -rho_0 / (2 * Area * diameter) * lambda(q) * Aux::smooth_abs(q) * q / rho;
    }
    return source;
  }

  Eigen::Matrix2d
  Isothermaleulerequation::derivative_sourceterm(Eigen::Vector2d const &state) {
    double rho = state[0];
    double q = state[1];

    Eigen::Matrix2d dsource;
    dsource(0, 0) = 0;
    dsource(0, 1) = 0;
    if(Reynolds(q)<2000){
      dsource(1, 0) = rho_0 / (2 * Area * diameter) * 64/coeff_of_Reynolds() * q / (rho * rho);
      dsource(1, 1) =
          -rho_0 / Area * 64 / coeff_of_Reynolds() / (2 * diameter) / rho;
    } else {
      dsource(1, 0) = rho_0 / (2 * Area * diameter) * lambda(q) *
                      Aux::smooth_abs(q) * q / (rho * rho);

      dsource(1,1) = -rho_0 / (2 * Area * diameter) / rho *
                   (
                    derivative_lambda(q) * Aux::smooth_abs(q) * q +
                    lambda(q) * Aux::derivative_smooth_abs(q) * q +
                    lambda(q) * Aux::smooth_abs(q)
                    );
    }
    return dsource;
  }

  Eigen::Vector2d
  Isothermaleulerequation::p_qvol(Eigen::Vector2d const &state) const {
    double rho = state[0];
    double q = state[1];

    Eigen::Vector2d p_qvol;

    p_qvol[0] = p(rho);
    p_qvol[1] = q;

    return p_qvol;
  }

  Eigen::Vector2d
  Isothermaleulerequation::state(Eigen::Vector2d const &p_qvol) const {

    double p = p_qvol[0];
    double q = p_qvol[1];
    Eigen::Vector2d state;
    state[0] = rho(p);
    state[1] = q;
    return state;
  }

  double Isothermaleulerequation::p(double rho) const {
    double p;
    p = c_vac_squared * rho / (1 - alpha * c_vac_squared * rho);
    return p;
  }
  double Isothermaleulerequation::derivative_p(double rho) const {
    double dp;
    double D = (1 - alpha * c_vac_squared * rho);
    dp = c_vac_squared / D * D;
    return dp;
  }

  double Isothermaleulerequation::rho(double p) const {
    double rho;
    rho = p / (c_vac_squared * (1 + alpha * p));
    return rho;
  }
  double Isothermaleulerequation::lambda(double Re) const{

    if(Re< 1e-8) {return 0;};
    if (Re < 2000) {
      return 64.0 / Re;
    } else if(Re >4000){
      return Swamee_Jain(Re);
    } else {
      Eigen::Vector4d monomials(Re*Re*Re,Re*Re,Re,1);
      return coefficients.dot(monomials);
    }
  }

  double Isothermaleulerequation::derivative_lambda(double Re) const {

    if (Re < 1e-8) {
      gthrow({"The source term must ensure that this case of Re<1e-8 doesn't happen!"});
    }
    if (Re < 2000) {
      return -64.0 / (Re*Re);
    } else if (Re > 4000) {
      return derivative_Swamee_Jain(Re);
    } else {
      Eigen::Vector4d monomials(3*Re *  Re, 2 * Re, 1, 0);
      return coefficients.dot(monomials);
    }
  }

  double Isothermaleulerequation::Reynolds(double q) const {
    double Re;
    Re = diameter / eta * rho_0 * Aux::smooth_abs(q);
    return Re;
  }

  double Isothermaleulerequation::coeff_of_Reynolds() const {
    double coeff;
    coeff = diameter / eta * rho_0;
    return coeff;
  }

  double Isothermaleulerequation::derivative_Reynolds(double q) const {
    double Re;
    Re = diameter / eta * rho_0 * Aux::derivative_smooth_abs(q);
    return Re;
  }

  double Isothermaleulerequation::Swamee_Jain(double Re) const {
    if(Re<4000) {
      gthrow({"This function must not be called for Reynolds numbers smaller than 4000!"});
    }

    double aux = log(roughness / (3.7 * diameter) + 5.74 / (pow(Re, 0.9)));
    double lambda = 0.25 / (aux*aux);
    return lambda;
  }

  double Isothermaleulerequation::derivative_Swamee_Jain(double Re) const {
    if (Re < 4000) {
      gthrow({"This function must not be called for Reynolds numbers smaller "
              "than 4000!"});
    }
    double x = exp(log(Re) * 0.9);
    double a = roughness / (3.7 * diameter) + 5.74 / x;
    double aux = log(a);
    double daux_dRe = 1/a *(-5.74/(x*x))*0.9*x/Re;
    double dlambda_dRe = -0.5 / (aux * aux * aux)*daux_dRe;
    return dlambda_dRe;    
  }

  } // namespace Model::Balancelaw
