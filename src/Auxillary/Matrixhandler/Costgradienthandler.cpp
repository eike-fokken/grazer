#include "Costgradienthandler.hpp"

namespace Aux {

  Costgradienthandler::Costgradienthandler(
      Eigen::Ref<Eigen::VectorXd> _gradient) :
      gradient(_gradient) {
    gradient.setZero();
  }

  void Costgradienthandler::add_to_coefficient(int row, double value) {
    gradient[row] += value;
  }
} // namespace Aux
