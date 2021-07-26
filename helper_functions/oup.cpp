/** @file */

#include "Normaldistribution.hpp"
#include "Stochastic.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>

namespace fs = std::filesystem;

/** \brief Simulates an Ornstein-Uhlenbeck-process with the Euler Maruyama
 * method.
 *
 * All arguments are optional. If one or more arguments is given, the first one
 * is given as the common substring with which all output files start. If
 * two arguments are given, the second argument is the filename of the json
 * file, get_mean_sigma should create, to output its computations. The default
 * value of the substring is "output". The default value of the filename is
 * "mean_and_sigma.json".
 */

int main(int argc, char **argv) {
  using namespace Aux;
  if (argc != 9) {
    std::cout << "Wrong number of arguments" << std::endl;
    return EXIT_FAILURE;
  }

  double start_time = std::stod(argv[1]);
  double end_time = std::stod(argv[2]);
  int number_of_time_steps = std::stoi(argv[3]);
  double mu = std::stod(argv[4]);
  double theta = std::stod(argv[5]);
  double sigma = std::stod(argv[6]);
  double stability_parameter = std::stod(argv[7]);
  std::string output_filename = argv[8];

  double delta_t_candidate = (end_time - start_time) / (number_of_time_steps);

  std::array<uint32_t, Aux::pcg_seed_count> used_seed;
  Normaldistribution distribution(used_seed);

  auto number_of_oup_steps = compute_stable_number_of_oup_steps(
      stability_parameter, theta, delta_t_candidate, 1);

  auto delta_t = delta_t_candidate / number_of_oup_steps;

  int number_of_time_points
      = static_cast<int>((end_time - start_time) / delta_t);

  fs::path outputfile(output_filename);
  std::ofstream stream(outputfile);

  stream << "time,value\n";

  double time = start_time;
  double last_value = mu;

  stream << time << "," << last_value << "\n";
  for (int i = 0; i != number_of_time_points; ++i) {
    time += delta_t;
    last_value = euler_maruyama_oup(
        stability_parameter, last_value, theta, mu, delta_t, sigma,
        distribution, number_of_oup_steps);
    stream << time << "," << last_value << "\n";
  }
  return EXIT_SUCCESS;
}
