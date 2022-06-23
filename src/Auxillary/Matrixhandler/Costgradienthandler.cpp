#include "Costgradienthandler.hpp"
#include "Exception.hpp"

namespace Aux {

  Costgradienthandler::Costgradienthandler(
      Eigen::Ref<Eigen::VectorXd> _gradient) :
      gradient(_gradient) {

#ifndef NDEBUG
    if (!_gradient.isZero()) {
      gthrow({"Costgradienthandler was given a non-Zero array!"});
    }
#endif
  }

  void Costgradienthandler::add_to_coefficient(int row, double value) {
    gradient[row] += value;
  }
} // namespace Aux
