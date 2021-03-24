#include <Exception.hpp>
#include <Isothermaleulerequation.hpp>
#include <Mathfunctions.hpp>

namespace Model::Balancelaw {

  Eigen::Matrix4d const Isothermaleulerequation::set_coeff_helper_matrix() {
    double a = laminar_border;
    double b = turbulent_border;

    Eigen::Matrix4d mat;
    mat << a * a * a, a * a, a, 1, //
        3 * a * a, 2 * a, 1, 0, //
        b * b * b, b * b, b, 1, //
        3 * b * b, 2 * b, 1, 0;
    return mat;
  }

  Eigen::Matrix4d const Isothermaleulerequation::coeff_helper_matrix = set_coeff_helper_matrix();

  Eigen::Vector4d Isothermaleulerequation::get_coefficients(double diameter,double roughness){

    Eigen::Vector4d spline_constraints(
        64. / laminar_border, -64. / (laminar_border * laminar_border),
        Swamee_Jain(turbulent_border, diameter, roughness),
        dSwamee_Jain_dRe(turbulent_border, diameter, roughness));
    return coeff_helper_matrix.lu().solve(spline_constraints);
    
    
  }

  
  Eigen::Vector2d
  Isothermaleulerequation::flux(Eigen::Vector2d const &state, double diameter) {

    double rho = state[0];
    double q = state[1];

    Eigen::Vector2d flux;

    auto Area = Aux::circle_area(0.5 * diameter);
    flux[0] = rho_0 / Area * q;
    flux[1] = Area / rho_0 * p(rho) +(rho_0 / Area) * q * q / rho;
    return flux;
  }

  Eigen::Matrix2d
  Isothermaleulerequation::dflux_dstate(Eigen::Vector2d const &state, double diameter) {

    double rho = state[0];
    double q = state[1];

    Eigen::Matrix2d dflux;
    auto Area = Aux::circle_area(0.5 * diameter);
    dflux(0, 0) = 0.0;
    dflux(0, 1) = rho_0 / Area;
    dflux(1, 0) = Area / rho_0 * dp_drho(rho) - rho_0 / Area * q * q / (rho * rho);
    dflux(1, 1) = 2 * rho_0 / Area * q / rho;
    return dflux;
  }

  Eigen::Vector2d
  Isothermaleulerequation::source(Eigen::Vector2d const &state, double diameter, double roughness){

    double rho = state[0];
    double q = state[1];
    double Re = Reynolds(q,diameter);

    Eigen::Vector2d source;
    source[0] = 0.0;
    auto Area = Aux::circle_area(0.5 * diameter);
    if (Re < laminar_border) { // laminar, that is linear friction:
      source[1] = -rho_0 / (2 * Area* diameter) * 64 / coeff_of_Reynolds(diameter) * q / rho;
    } else { // transitional and turbulent friction:
      source[1] = -rho_0 / (2 * Area * diameter) * lambda_non_laminar(Re,diameter,roughness) * std::abs(q) * q / rho;
    }
    return source;
  }

  Eigen::Matrix2d
  Isothermaleulerequation::dsource_dstate(Eigen::Vector2d const &state,
                                          double diameter,double roughness){
    double rho = state[0];
    double q = state[1];

    double Re = Reynolds(q,diameter);
    Eigen::Matrix2d dsource;
    dsource(0, 0) = 0;
    dsource(0, 1) = 0;

    auto Area = Aux::circle_area(0.5 * diameter);

    if (Re < laminar_border) { // laminar, that is, linear friction:
      dsource(1, 0) = rho_0 / (2 * Area * diameter) * 64/coeff_of_Reynolds(diameter) * q / (rho * rho);
      dsource(1, 1) =
          -rho_0 / (2 * Area * diameter) * 64 / coeff_of_Reynolds(diameter) / rho;
    }
    else { // transitional and turbulent friction:
      dsource(1, 0) = rho_0 / (2 * Area * diameter) * lambda_non_laminar(Re,diameter,roughness) *
        std::abs(q) * q / (rho * rho);

      dsource(1,1) = -rho_0 / (2 * Area * diameter) / rho *
                   (
                    dlambda_non_laminar_dRe(Re,diameter,roughness) *dReynolds_dq(q,diameter) * std::abs(q) * q +
                    lambda_non_laminar(Re,diameter,roughness) * Aux::dabs_dx(q) * q +
                    lambda_non_laminar(Re,diameter,roughness) * std::abs(q)
                    );
    }
    return dsource;
  }

  Eigen::Vector2d
  Isothermaleulerequation::p_qvol(Eigen::Vector2d const &state){
    double rho = state[0];
    double q = state[1];

    Eigen::Vector2d p_qvol;

    p_qvol[0] = p(rho);
    p_qvol[1] = q;

    return p_qvol;
  }

  Eigen::Matrix2d Isothermaleulerequation::dp_qvol_dstate(Eigen::Vector2d const &state){
    double rho = state[0];

    Eigen::Matrix2d dp_qvol;
    dp_qvol(0,0) = dp_drho( rho);
    dp_qvol(0, 1) = 0.0;
    dp_qvol(1, 0) = 0.0;
    dp_qvol(1, 1) = 1.0;
    return dp_qvol;
  }

  Eigen::Vector2d
  Isothermaleulerequation::state(Eigen::Vector2d const &p_qvol){

    double p = p_qvol[0];
    double q = p_qvol[1];
    Eigen::Vector2d state;
    state[0] = rho(p);
    state[1] = q;
    return state;
  }

  
  Eigen::Vector2d Isothermaleulerequation::p_qvol_from_p_qvol_bar(Eigen::Vector2d const &p_qvol_bar){
    double p_bar = p_qvol_bar[0];
    double q = p_qvol_bar[1];
    Eigen::Vector2d p_qvol;
    p_qvol[0] = p_pascal_from_p_bar(p_bar);
    p_qvol[1] = q;
    return p_qvol;
  }

  double
  Isothermaleulerequation::p_pascal_from_p_bar(double p){
    return p*bar;
  }


  double
  Isothermaleulerequation::p_bar_from_p_pascal(double p){
    return p/bar;
  }

  double
  Isothermaleulerequation::dp_bar_from_p_pascal_dp_pascal(double){
    return 1.0/bar;
  }


  Eigen::Vector2d Isothermaleulerequation::p_qvol_bar_from_p_qvol(Eigen::Vector2d const &p_qvol){
    double p = p_qvol[0];
    double q = p_qvol[1];
    Eigen::Vector2d p_qvol_bar;
    p_qvol_bar[0] = p_bar_from_p_pascal(p);
    p_qvol_bar[1] = q;
    return p_qvol_bar;
  }

  Eigen::Matrix2d Isothermaleulerequation::dp_qvol_bar_from_p_qvold_p_qvol(Eigen::Vector2d const &p_qvol){
    double p_pascal = p_qvol[0];
    Eigen::Matrix2d dp_qvol;
    dp_qvol(0,0) = dp_bar_from_p_pascal_dp_pascal(p_pascal);
    dp_qvol(0, 1) = 0.0;
    dp_qvol(1, 0) = 0.0;
    dp_qvol(1, 1) = 1.0;
    return dp_qvol;
  }

  double Isothermaleulerequation::p(double rho){
    double p;
    p = c_vac_squared * rho / (1 - alpha * c_vac_squared * rho);
    return p;
  }
  double Isothermaleulerequation::dp_drho(double rho){
    double dp;
    double D = (1 - alpha * c_vac_squared * rho);
    dp = c_vac_squared / (D * D);
    return dp;
  }

  double Isothermaleulerequation::rho(double p){
    double rho;
    rho = p / (c_vac_squared * (1 + alpha * p));
    return rho;
  }
  double Isothermaleulerequation::lambda_non_laminar(double Re,double diameter,double roughness){

    if(Re< laminar_border-Aux::EPSILON) {
      gthrow(
          {"This function is only valid for Reynolds numbers over and at 2000!",
           " For other values, manually insert "
           "\"64/Reynolds(q)\" instead"});
    }
    if(Re >turbulent_border){
      return Swamee_Jain(Re,diameter,roughness);
    } else {
      Eigen::Vector4d monomials(Re*Re*Re,Re*Re,Re,1);
      return get_coefficients(diameter,roughness).dot(monomials);
    }
  }

  double
  Isothermaleulerequation::dlambda_non_laminar_dRe(double Re, double diameter,
                                                   double roughness){

    if (Re < laminar_border-Aux::EPSILON) {gthrow({"This function is only valid for Reynolds numbers over and at 2000!",
        " For other values, manually insert \"-64/(Reynolds(q)*Reynolds(q))\" instead"});}

    if (Re > turbulent_border) {
      return dSwamee_Jain_dRe(Re,diameter,roughness);
    } else {
      Eigen::Vector4d monomials(3*Re *  Re, 2 * Re, 1, 0);
      return get_coefficients(diameter,roughness).dot(monomials);
    }
  }

  double Isothermaleulerequation::Reynolds(double q,
                                           double diameter){
    double Re;
    Re = diameter / eta * rho_0 * std::abs(q);
    return Re;
  }

  double Isothermaleulerequation::coeff_of_Reynolds(double diameter){
    double coeff;
    coeff = diameter / eta * rho_0;
    return coeff;
  }

  double Isothermaleulerequation::dReynolds_dq(double q,
                                               double diameter){

    if (Reynolds(q,diameter) < laminar_border-Aux::EPSILON) {
      gthrow(
             {"This function", __FUNCTION__ ,", should only be called for Reynolds "
                                         "numbers over and at 2000!"});
    }
    double dRe_dq;
    dRe_dq = diameter / eta * rho_0 * Aux::dabs_dx(q);
    return dRe_dq;
  }

  double Isothermaleulerequation::Swamee_Jain(double Re,
                                              double diameter, double roughness){
    if(Re<turbulent_border-Aux::EPSILON) {
      gthrow({"This function must not be called for Reynolds numbers smaller than 4000!"});
    }

    double aux = log10(roughness / (3.7 * diameter) + 5.74 / (pow(Re, 0.9)));
    double lambda = 0.25 / (aux*aux);
    return lambda;
  }

  double Isothermaleulerequation::dSwamee_Jain_dRe(double Re, double diameter,
                                                   double roughness){
    if (Re < turbulent_border-Aux::EPSILON) {
      gthrow({"This function must not be called for Reynolds numbers smaller "
              "than 4000!"});
    }
    double x = exp(log(Re) * 0.9);
    double a = roughness / (3.7 * diameter) + 5.74 / x;
    double aux = log10(a);
    double daux_dRe = 1/log(10)*a *(-5.74/(x*x))*0.9*x/Re;
    double dlambda_dRe = -0.5 / (aux * aux * aux)*daux_dRe;
    return dlambda_dRe;
  }

  double
  Isothermaleulerequation::exact_turbulent_lambda(double Re, double diameter,
                                                  double roughness){
    if (Re < turbulent_border-Aux::EPSILON) {
      gthrow({"This function must not be called for Reynolds numbers smaller "
              "than 4000!"});
    }
    int counter;
    double lambda = Swamee_Jain(Re,diameter,roughness);
    double mu = 1/sqrt(lambda);
    for (counter = 0; counter !=10; ++counter) {
      double inner = 2.51 / Re * mu + roughness / (3.71*diameter);
      double dinner_dmu = 2.51/Re;
      double f= mu +2*log10( inner);
      double df = 1+2/log(10)*dinner_dmu/inner;
      mu = mu - f/df;
      if(f < 1e-12) {break;}
    }

    return 1/(mu*mu);
  }

  } // namespace Model::Balancelaw
