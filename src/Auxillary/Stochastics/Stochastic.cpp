#include "Stochastic.hpp"
#include "Exception.hpp"
#include <string>

namespace Aux {

  int compute_stable_number_of_oup_steps(
      double stability_parameter, double theta, double delta_t,
      int number_of_stochastic_steps) {
    if (stability_parameter <= 0 or stability_parameter >= 2) {
      gthrow(
          {"The stability parameter a must fulfill 0 < a < 2. Actual value: ",
           std::to_string(stability_parameter)});
    }

    double stochastic_stepsize = delta_t / number_of_stochastic_steps;
    if (theta <0){
      gthrow({"The parameter theta must fulfill theta > 0. Actual value: ",
           std::to_string(theta)});
    }
    if (number_of_stochastic_steps < 0) {
      gthrow(
          {"The number of stochstic time steps must fulfill N > 0. Actual value: ",
           std::to_string(number_of_stochastic_steps)});
    }

      if (theta * stochastic_stepsize >= stability_parameter) {
        number_of_stochastic_steps
            = static_cast<int>(
                  std::ceil(theta * delta_t / (stability_parameter)))
              + 1;
      }
    return number_of_stochastic_steps;
  }
} // namespace Aux
