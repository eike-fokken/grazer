#include "Stochastic.hpp"
#include "Normaldistribution.hpp"

namespace Aux {
  double euler_maruyama_oup(
      double last_value, double theta, double mu, double delta_t, double sigma,
      Normaldistribution &distribution, int number_of_stochastic_steps) {
    double stochastic_stepsize = delta_t / number_of_stochastic_steps;
    double current_value = last_value;
    for (auto i = 0; i != number_of_stochastic_steps; ++i) {
      current_value = current_value
                      + theta * (mu - current_value) * stochastic_stepsize
                      + sigma * distribution(sqrt(stochastic_stepsize));
    }
    // std::cout << "mean: " << mu << "\n";
    // std::cout << "computed: " << current_value << "\n";
    return current_value;
  }
} // namespace Aux
