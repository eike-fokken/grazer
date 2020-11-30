#include <Eigen/Sparse>
#include <Eigen/Dense>


namespace Model::Balancelaw {

class Isothermaleulerequation {



  Eigen::Vector2d flux(Eigen::Vector2d const & state);

  Eigen::Matrix2d flux_derivative(Eigen::Vector2d const & state);

  Eigen::Vector2d sourceterm(Eigen::Vector2d const & state);


  Eigen::Matrix2d sourceterm_derivative(Eigen::Vector2d const & state);


};

}
