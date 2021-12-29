#pragma once
#include <Eigen/Dense>

namespace Aux {
  class Costgradienthandler final {
  public:
    Costgradienthandler(
        Eigen::Ref<Eigen::VectorXd> gradient, bool first_invocation = true);
    void set_coefficient(Eigen::Index row, double value);
    void add_to_coefficient(Eigen::Index row, double value);

  private:
    Eigen::Ref<Eigen::VectorXd> gradient;
  };
} // namespace Aux
