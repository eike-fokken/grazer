#include <Exception.hpp>
#include <Isothermaleulerequation.hpp>
#include <Mathfunctions.hpp>
#include <cmath>
#include <string>

namespace Model::Balancelaw {

  Isothermaleulerequation::Isothermaleulerequation(double _Area,
                                                   double _diameter,
                                                   double _roughness)
      : Area(_Area), diameter(_diameter), roughness(_roughness) {
    double a = 2000;
    double b = 4000;

    Eigen::Matrix4d aux;
    aux << a * a * a, a * a, a, 1, //
        3 * a * a, 2 * a, 1, 0,  //
        b * b * b, b * b, b, 1,  //
        3 * b * b, 2 * b, 1, 0;  //
    Eigen::Vector4d spline_constraints(64. / a,-64./(a*a), Swamee_Jain(b), dSwamee_Jain_dRe(b));
    coefficients = aux.lu().solve(spline_constraints);
  }

  Eigen::Vector2d
  Isothermaleulerequation::flux(Eigen::Vector2d const &state) const {

    double rho = state[0];
    double q = state[1];

    Eigen::Vector2d flux;

    flux[0] = rho_0 / Area * q;
    flux[1] = Area / rho_0 * p(rho) +(rho_0 / Area) * q * q / rho;
    return flux;
  }

  Eigen::Matrix2d
  Isothermaleulerequation::dflux_dstate(Eigen::Vector2d const &state) const {

    double rho = state[0];
    double q = state[1];

    Eigen::Matrix2d dflux;

    dflux(0, 0) = 0.0;
    dflux(0, 1) = rho_0 / Area;
    dflux(1, 0) = Area / rho_0 * dp_drho(rho) - rho_0 / Area * q * q / (rho * rho);
    dflux(1, 1) = 2 * rho_0 / Area * q / rho;
    return dflux;
  }

  Eigen::Vector2d
  Isothermaleulerequation::source(Eigen::Vector2d const &state) const{

    double rho = state[0];
    double q = state[1];
    double Re = Reynolds(q);

    Eigen::Vector2d source;
    source[0] = 0.0;

    if (Re < 2000) { // laminar, that is linear friction:
      source[1] = -rho_0 / (2 * Area* diameter) * 64 / coeff_of_Reynolds() * q / rho;
    } else { // transitional and turbulent friction:
      source[1] = -rho_0 / (2 * Area * diameter) * lambda_non_laminar(Re) * std::abs(q) * q / rho;
    }
    return source;
  }

  Eigen::Matrix2d
  Isothermaleulerequation::dsource_dstate(Eigen::Vector2d const &state) const {
    double rho = state[0];
    double q = state[1];

    double Re = Reynolds(q);
    Eigen::Matrix2d dsource;
    dsource(0, 0) = 0;
    dsource(0, 1) = 0;
    if (Re < 2000) { // laminar, that is, linear friction:
      dsource(1, 0) = rho_0 / (2 * Area * diameter) * 64/coeff_of_Reynolds() * q / (rho * rho);
      dsource(1, 1) =
          -rho_0 / (2 * Area * diameter) * 64 / coeff_of_Reynolds() / rho;
    } else { // transitional and turbulent friction:
      dsource(1, 0) = rho_0 / (2 * Area * diameter) * lambda_non_laminar(Re) *
        std::abs(q) * q / (rho * rho);

      dsource(1,1) = -rho_0 / (2 * Area * diameter) / rho *
                   (
                    dlambda_non_laminar_dRe(Re) *dReynolds_dq(q) * std::abs(q) * q +
                    lambda_non_laminar(Re) * Aux::dabs_dx(q) * q +
                    lambda_non_laminar(Re) * std::abs(q)
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

  Eigen::Matrix2d Isothermaleulerequation::dp_qvol_dstate(Eigen::Vector2d const &state) const{
    double rho = state[0];

    Eigen::Matrix2d dp_qvol;
    dp_qvol(0,0) = dp_drho( rho);
    dp_qvol(0, 1) = 0.0;
    dp_qvol(1, 0) = 0.0;
    dp_qvol(1, 1) = 1.0;
    return dp_qvol;
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

  double
  Isothermaleulerequation::p_pascal_from_p_bar(double p) const {
    return p*bar;
  }

  Eigen::Vector2d Isothermaleulerequation::p_qvol_from_p_qvol_bar(Eigen::Vector2d const &p_qvol_bar) const {
    double p_bar = p_qvol_bar[0];
    double q = p_qvol_bar[1];
    Eigen::Vector2d p_qvol;
    p_qvol[0] = p_pascal_from_p_bar(p_bar);
    p_qvol[1] = q;
    return p_qvol;
  }

  double
  Isothermaleulerequation::p_bar_from_p_pascal(double p) const {
    return p/bar;
  }

  Eigen::Vector2d Isothermaleulerequation::p_qvol_bar_from_p_qvol(Eigen::Vector2d const &p_qvol) const {
    double p = p_qvol[0];
    double q = p_qvol[1];
    Eigen::Vector2d p_qvol_bar;
    p_qvol_bar[0] = p_bar_from_p_pascal(p);
    p_qvol_bar[1] = q;
    return p_qvol_bar;
  }


  

  double Isothermaleulerequation::p(double rho) const {
    double p;
    p = c_vac_squared * rho / (1 - alpha * c_vac_squared * rho);
    return p;
  }
  double Isothermaleulerequation::dp_drho(double rho) const {
    double dp;
    double D = (1 - alpha * c_vac_squared * rho);
    dp = c_vac_squared / (D * D);
    return dp;
  }

  double Isothermaleulerequation::rho(double p) const {
    double rho;
    rho = p / (c_vac_squared * (1 + alpha * p));
    return rho;
  }
  double Isothermaleulerequation::lambda_non_laminar(double Re) const{

    if(Re< 2000) {
      gthrow(
          {"This function is only valid for Reynolds numbers over and at 2000!",
           " For other values, manually insert "
           "\"64/Reynolds(q)\" instead"});
    }
    if(Re >4000){
      return Swamee_Jain(Re);
    } else {
      Eigen::Vector4d monomials(Re*Re*Re,Re*Re,Re,1);
      return coefficients.dot(monomials);
    }
  }

  double Isothermaleulerequation::dlambda_non_laminar_dRe(double Re) const {

    if (Re < 2000) {gthrow({"This function is only valid for Reynolds numbers over and at 2000!",
        " For other values, manually insert \"-64/(Reynolds(q)*Reynolds(q))\" instead"});}

    if (Re > 4000) {
      return dSwamee_Jain_dRe(Re);
    } else {
      Eigen::Vector4d monomials(3*Re *  Re, 2 * Re, 1, 0);
      return coefficients.dot(monomials);
    }
  }

  double Isothermaleulerequation::Reynolds(double q) const {
    double Re;
    Re = diameter / eta * rho_0 * std::abs(q);
    return Re;
  }

  double Isothermaleulerequation::coeff_of_Reynolds() const {
    double coeff;
    coeff = diameter / eta * rho_0;
    return coeff;
  }

  double Isothermaleulerequation::dReynolds_dq(double q) const {

    if (Reynolds(q) < 2000) {
      gthrow(
             {"This function", __FUNCTION__ ,", should only be called for Reynolds "
                                         "numbers over and at 2000!"});
    }
    double dRe_dq;
    dRe_dq = diameter / eta * rho_0 * Aux::dabs_dx(q);
    return dRe_dq;
  }

  double Isothermaleulerequation::Swamee_Jain(double Re) const {
    if(Re<4000) {
      gthrow({"This function must not be called for Reynolds numbers smaller than 4000!"});
    }

    double aux = log(roughness / (3.7 * diameter) + 5.74 / (pow(Re, 0.9)));
    double lambda = 0.25 / (aux*aux);
    return lambda;
  }

  double Isothermaleulerequation::dSwamee_Jain_dRe(double Re) const {
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
