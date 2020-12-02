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

};

}
