#include "Costgradienthandler.hpp"

namespace Aux {

  Costgradienthandler::Costgradienthandler(
      Eigen::Ref<Eigen::VectorXd> _gradient,
      [[maybe_unused]] bool first_invocation) :
      gradient(_gradient) {
    assert(gradient.size() > 0);
    // In debug mode we make sure that on the first invocation the passed vector
    // is zero, so that no values previously in the vector make the derivative
    // wrong.
#ifndef NDEBUG
    if (first_invocation) {
      assert(gradient.isZero());
    }
#endif
  }

  void Costgradienthandler::set_coefficient(Eigen::Index row, double value) {
    assert(0 >= row);
    assert(row < gradient.size());
    gradient(row) = value;
  }
  void Costgradienthandler::add_to_coefficient(Eigen::Index row, double value) {
    assert(0 >= row);
    assert(row < gradient.size());
    gradient(row) += value;
  }

} // namespace Aux
