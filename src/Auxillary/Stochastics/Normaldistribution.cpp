#include "Normaldistribution.hpp"
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
  Normaldistribution::operator()(double mean, double standard_deviation) {
    std::normal_distribution<>::param_type parameters(mean, standard_deviation);
    return dist(generator, parameters);
  }

  double Normaldistribution::operator()(double standard_deviation) {
    return (*this)(0, standard_deviation);
  }

} // namespace Aux
