#include "Normaldistribution.hpp"
#include "pcg_extras.hpp"
#include "randutils.hpp"
#include <bits/stdint-uintn.h>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <map>
#include <random>
#include <string>

namespace Aux {

  pcg64 setup_random_number_generator() {

    // std::random_device ro;

    // auto args
    //     = {static_cast<uint32_t>(std::chrono::high_resolution_clock::now()
    //                                  .time_since_epoch()
    //                                  .count()),
    //        static_cast<uint32_t>(ro()),
    //        static_cast<uint32_t>(ro()),
    //        static_cast<uint32_t>(ro()),
    //        static_cast<uint32_t>(ro()),
    //        static_cast<uint32_t>(ro()),
    //        static_cast<uint32_t>(ro()),
    //        static_cast<uint32_t>(ro())};
    // randutils::auto_seed_256 seed_source(args);
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
