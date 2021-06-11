#include "Normaldistribution.hpp"
#include "Exception.hpp"
#include "pcg_extras.hpp"
#include "randutils.hpp"

#include <iostream>
#include <map>
#include <random>
#include <string>

namespace Aux {

  std::array<uint32_t, pcg_seed_count> make_random_seed() {
    std::array<uint32_t, pcg_seed_count> seed;
    randutils::auto_seeded<randutils::seed_seq_fe<pcg_seed_count, uint32_t>>
        seed_source;
    seed_source.param(seed.begin());
    return seed;
  }

  pcg64 setup_random_number_generator(
      std::array<uint32_t, pcg_seed_count> &seed_essence) {

    randutils::seed_seq_fe<pcg_seed_count, uint32_t> seed_source(
        seed_essence.begin(), seed_essence.end());

    pcg64 rng(seed_source);

    return rng;
  }

  Normaldistribution::Normaldistribution(
      std::array<uint32_t, pcg_seed_count> &used_seed,
      std::array<uint32_t, pcg_seed_count> seed) :
      generator(setup_random_number_generator(seed)) {
    used_seed = seed;
  }

  double
  Normaldistribution::get_sample(double mean, double standard_deviation) {
    std::normal_distribution<>::param_type parameters(mean, standard_deviation);
    return dist(generator, parameters);
  }

  double Normaldistribution::get_sample(double standard_deviation) {
    return get_sample(0, standard_deviation);
  }

  Truncatednormaldist::Truncatednormaldist(
      std::array<uint32_t, pcg_seed_count> &used_seed,
      std::array<uint32_t, pcg_seed_count> seed) :
      normaldist(used_seed, seed) {}

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
