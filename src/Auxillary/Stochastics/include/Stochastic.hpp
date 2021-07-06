#pragma once
#include "Exception.hpp"
#include <cmath>

namespace Aux {

  template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

  class Normaldistribution;

  template <typename Distribution>
  double euler_maruyama_oup(
      double last_value, double theta, double mu, double delta_t, double sigma,
      Distribution &distribution, int number_of_stochastic_steps) {
    double stochastic_stepsize = delta_t / number_of_stochastic_steps;
    double current_value = last_value;
    if (1 - theta * stochastic_stepsize < -1) {

      // stability_threshold must fulfill 0 < stability_threshold < 2 to
      // guarantee stability in mean of the Euler Mauryan process. The lower the
      // number, the more steps must be taken, but also the more stable the
      // process is.
      double stability_threshold = 0.2;
      number_of_stochastic_steps
          = static_cast<int>(std::ceil(theta * delta_t / (stability_threshold)))
            + 1;
      stochastic_stepsize = delta_t / (number_of_stochastic_steps - 1);
      // Probably we should notify the user of the actually used stochastic
      // stepsize!
    }
    for (auto i = 0; i != number_of_stochastic_steps; ++i) {
      current_value
          = current_value + theta * (mu - current_value) * stochastic_stepsize
            + sigma * distribution.get_sample(sqrt(stochastic_stepsize));
      double difference = 0.4;
      if (sgn(mu) * current_value < (1.0 - difference) * sgn(mu) * mu) {
        current_value = (1.0 - difference) * mu;
      } else if (sgn(mu) * current_value > (1.0 + difference) * sgn(mu) * mu) {
        current_value = (1.0 + difference) * mu;
      }
    }
    return current_value;
  }

  template <typename Distribution>
  double euler_maruyama_oup_relative(
      double last_value, double theta, double mu, double delta_t, double sigma,
      Distribution &distribution, int number_of_stochastic_steps) {
    double stochastic_stepsize = delta_t / number_of_stochastic_steps;
    double current_value = last_value;
    for (auto i = 0; i != number_of_stochastic_steps; ++i) {
      current_value
          = current_value + theta * (mu - current_value) * stochastic_stepsize
            + mu * sigma * distribution.get_sample(sqrt(stochastic_stepsize));
    }
    // std::cout << "mean: " << mu << "\n";
    // std::cout << "computed: " << current_value << "\n";
    double difference = 0.43;
    if (sgn(mu) * current_value < (1.0 - difference) * sgn(mu) * mu) {
      return (1.0 - difference) * mu;
    } else if (sgn(mu) * current_value > (1.0 + difference) * sgn(mu) * mu) {
      return (1.0 + difference) * mu;
    } else {
      return current_value;
    }
  }

} // namespace Aux
