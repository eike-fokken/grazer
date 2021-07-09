#pragma once
#include "Exception.hpp"
#include <cmath>

namespace Aux {

  template <typename T> int sgn(T val) { return (T(0) < val) - (val < T(0)); }

  class Normaldistribution;

  /// \brief Compute a stable number of stochastic timesteps on the interval
  /// delta_t for the Euler-Maruyama method for the OUP.
  int compute_stable_number_of_oup_steps(
      double stability_parameter, double theta, double delta_t,
      int number_of_stochastic_steps);

  /// \brief Simulate a number of Ornstein-Uhlenbeck-process steps.
  ///
  /// Uses the Euler-Maruyama-method to simulate the OUP. Note that no stability
  /// checking is done!
  /// @param last_value value at last time step.
  /// @param theta the EM parameter for the deterministic part
  /// @param mu The mean, towards which the deterministic part converges
  /// @param delta_t the time step of the outer process (not of the EM!)
  /// @param sigma the EM parameter for the stochastic part
  /// @param distribution A normal distribution
  /// @param number_of_stochastic_steps The stochastic time step is given as
  /// delta_t / number_of_stochastic_steps
  /// @returns The value of the process after the last step.
  template <typename Distribution>
  double euler_maruyama_oup(
      double stability_parameter, double last_value, double theta, double mu,
      double delta_t, double sigma, Distribution &distribution,
      int number_of_stochastic_steps) {
    number_of_stochastic_steps = compute_stable_number_of_oup_steps(
        stability_parameter, theta, delta_t, number_of_stochastic_steps);
    double stochastic_stepsize = delta_t / number_of_stochastic_steps;
    double current_value = last_value;
    for (auto i = 0; i != number_of_stochastic_steps; ++i) {
      current_value
          = current_value + theta * (mu - current_value) * stochastic_stepsize
            + sigma * distribution.get_sample(sqrt(stochastic_stepsize));
      // double difference = 0.4;
      // if (sgn(mu) * current_value < (1.0 - difference) * sgn(mu) * mu) {
      //   current_value = (1.0 - difference) * mu;
      // } else if (sgn(mu) * current_value > (1.0 + difference) * sgn(mu) * mu)
      // {
      //   current_value = (1.0 + difference) * mu;
      // }
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
