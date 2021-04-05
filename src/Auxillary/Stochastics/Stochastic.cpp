#include "Stochastic.hpp"
#include "Normaldistribution.hpp"

namespace Aux {
  double euler_maruyama_step(
      double last_value, double theta, double mu, double delta_t, double sigma,
      Normaldistribution &distribution) {
    // Careful: delta_t is the variance, to get the standard deviation we must
    // take the squareroot!
    return last_value + theta * (mu - last_value) * delta_t
           + sigma * distribution(sqrt(delta_t));
  }
} // namespace Aux
