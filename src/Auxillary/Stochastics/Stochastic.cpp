#include "Stochastic.hpp"
#include "Normaldistribution.hpp"

namespace Aux {
  double euler_maruyama_step(
      double last_value, double theta, double mu, double delta_t, double sigma,
      Normaldistribution &distribution, int number_of_stochastic_steps) {
    double stochastic_stepsize = delta_t / number_of_stochastic_steps;
    double current_value = last_value;
    for (auto i = 0; i != number_of_stochastic_steps; ++i) {
      current_value
          = current_value
                * (1 - theta * mu * current_value * stochastic_stepsize)
            + theta * mu * stochastic_stepsize
            + sigma * distribution(sqrt(stochastic_stepsize));
    }
    return current_value;
  }
} // namespace Aux
