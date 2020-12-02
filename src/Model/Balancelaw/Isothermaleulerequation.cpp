#include <Isothermaleulerequation.hpp>

namespace Model::Balancelaw {

  Eigen::Vector2d
  Isothermaleulerequation::flux(Eigen::Vector2d const &state) const{}

  Eigen::Matrix2d
  Isothermaleulerequation::flux_derivative(Eigen::Vector2d const &state) const{}

  Eigen::Vector2d
  Isothermaleulerequation::sourceterm(Eigen::Vector2d const &state) const{}

  Eigen::Matrix2d Isothermaleulerequation::derivative_sourceterm(Eigen::Vector2d const &state) const{}

  Eigen::Vector2d
  Isothermaleulerequation::state_2_p_qvol(Eigen::Vector2d const &state) const{}

  Eigen::Vector2d
  Isothermaleulerequation::p_qvol_2_state(Eigen::Vector2d const &state) const{}

  double Isothermaleulerequation::p_2_rho(double p){}
  double Isothermaleulerequation::rho_2_p(double rho){}

} // namespace Model::Balancelaw
