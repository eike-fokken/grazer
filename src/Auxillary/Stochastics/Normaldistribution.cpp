#include "Normaldistribution.hpp"
#include "Exception.hpp"
#include "pcg_extras.hpp"
#include "randutils.hpp"
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>

namespace Aux {

  pcg64 setup_random_number_generator() {

    randutils::auto_seed_256 seed_source;
    pcg64 rng(seed_source);
    return rng;
  }

  double
  Normaldistribution::get_sample(double mean, double standard_deviation) {
    std::normal_distribution<>::param_type parameters(mean, standard_deviation);
    return dist(generator, parameters);
  }

  double Normaldistribution::get_sample(double standard_deviation) {
    return get_sample(0, standard_deviation);
  }

  double Truncatednormaldist::get_sample(
      double mean, double standard_deviation, double lower_bound,
      double upper_bound) {
    int maximum_number_of_trials = 100000;
    double sample = normaldist.get_sample(mean, standard_deviation);
    for (int counter = 0; counter < maximum_number_of_trials; ++counter) {
      if (sample >= lower_bound and sample <= upper_bound) {
        return sample;
      }
      sample = normaldist.get_sample(mean, standard_deviation);
    }
    gthrow({
        "Failed to get a conforming sample in ",
        std::to_string(maximum_number_of_trials),
        " trials.\n",
        " Parameters were:\n",
        "mean: ",
        std::to_string(mean),
        "\n",
        "standard_deviation: ",
        std::to_string(standard_deviation),
        "\n",
        "lower_bound: ",
        std::to_string(lower_bound),
        "\n",
        "upper_bound: ",
        std::to_string(upper_bound),
        "\n",
    });
  }

  double
  Truncatednormaldist::get_sample(double mean, double standard_deviation) {
    double lower_bound = mean - 2 * standard_deviation;
    double upper_bound = mean + 2 * standard_deviation;
    return get_sample(mean, standard_deviation, lower_bound, upper_bound);
  }

  double Truncatednormaldist::get_sample(
      double standard_deviation, double lower_bound, double upper_bound) {
    double mean = 0;
    return get_sample(mean, standard_deviation, lower_bound, upper_bound);
  }

  double Truncatednormaldist::get_sample(double standard_deviation) {
    double mean = 0;
    double lower_bound = mean - 2 * standard_deviation;
    double upper_bound = mean + 2 * standard_deviation;
    return get_sample(mean, standard_deviation, lower_bound, upper_bound);
  }

} // namespace Aux
