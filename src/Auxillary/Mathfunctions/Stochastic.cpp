#include "Stochastic.hpp"
#include "Normaldistribution.hpp"

namespace Aux {
  double euler_maruyama(
      double last_value, double theta, double mu, double delta_t, double sigma,
      Normaldistribution &distribution) {
    return last_value + theta * (mu - last_value) * delta_t
           + sigma * distribution(delta_t);
  }
} // namespace Aux
