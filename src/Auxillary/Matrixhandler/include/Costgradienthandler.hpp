#pragma once
#include <Eigen/Dense>

namespace Aux {

  class Costgradienthandler final {
  public:
    Costgradienthandler(Eigen::Ref<Eigen::VectorXd> _gradient);

    void add_to_coefficient(int row, double value);

  private:
    Eigen::Ref<Eigen::VectorXd> gradient;
  };

} // namespace Aux
