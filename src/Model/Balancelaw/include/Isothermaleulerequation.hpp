#include <Eigen/Sparse>
#include <Eigen/Dense>



namespace Model::Balancelaw {

class Isothermaleulerequation {

public:

  Eigen::Vector2d flux(Eigen::Vector2d const & state) const;

  Eigen::Matrix2d flux_derivative(Eigen::Vector2d const & state) const;

  Eigen::Vector2d sourceterm(Eigen::Vector2d const & state) const;

  Eigen::Matrix2d derivative_sourceterm(Eigen::Vector2d const & state) const;

  Eigen::Vector2d state_2_p_qvol(Eigen::Vector2d const & state) const;

  Eigen::Vector2d p_qvol_2_state(Eigen::Vector2d const & state) const;

  double p_2_rho(double p);
  double rho_2_p(double rho);

  // These are physical constants except for the temperature T.
  // Later on it may be useful to change them to include other gases.

  static constexpr double bar = 1e5;
  static constexpr double rho_0 = 0.785;
  static constexpr double T_0 = 273.15;
  static constexpr double T_crit = 192.0;
  static constexpr double z_0 = 1.005;
  static constexpr double p_0 = 1.01325 * bar;
  static constexpr double p_crit = 46.4;
  static constexpr double T = 283.15;
  static constexpr double alpha = (0.257 / p_crit - 0.533 * T_crit / (p_crit * T));

  static constexpr double c_vac_squared = p_0 * T/(z_0*T_0*rho_0);
  };

}
