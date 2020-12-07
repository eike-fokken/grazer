#include <Eigen/Sparse>
#include <Exception.hpp>
#include <Jsonreader.hpp>
#include <Newtonsolver.hpp>
#include <Printguard.hpp>
#include <Problem.hpp>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char **argv) {

  if (argc != 4 and argc != 1) {
    gthrow({" Wrong number of arguments."})
  }

  std::filesystem::path topology;
  std::filesystem::path initial;
  std::filesystem::path boundary;
  if (argc == 4) {
    topology = argv[1];
    initial = argv[2];
    boundary = argv[3];
    if (!std::filesystem::is_regular_file(topology) or
        !std::filesystem::is_regular_file(initial) or
        !std::filesystem::is_regular_file(boundary)) {
      std::cout
          << "One of the given files is not a regular file, I will abort now."
          << std::endl;
      return 1;
    }
  } else {
    topology = "topology_pretty.json";
    initial = "initial_pretty.json";
    boundary = "boundary_pretty.json";
  }

  //   std::filesystem::path topology(argv[1]);
  // std::filesystem::path initial(argv[2]);
  // std::filesystem::path boundary(argv[3]);

  //////////////////////////////////////////////////
  ////////////////// SANITIZE INPUT FIRST!!!!
  //////////////////////////////////////////////////
  // Important task: validate the json before using it.

  std::filesystem::path output_dir("output");

  if (std::filesystem::exists(output_dir)) {
    if (!std::filesystem::is_directory(output_dir)) {
      std::cout << "The output directory \"output\"\n"
                   "is present, but not a directory, I will abort now."
                << std::endl;
      return 1;
    }
    auto ms_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch());
    std::string milli = std::to_string(ms_since_epoch.count());
    std::string moved_output_dir;
    moved_output_dir = output_dir.string();
    moved_output_dir.append("_");
    moved_output_dir.append(milli);
    std::filesystem::rename(output_dir.string(), moved_output_dir);
    std::cout << "moved old directory " << output_dir << " to "
              << moved_output_dir << std::endl;
    std::cout << "result files will be saved to " << output_dir << std::endl;
  }
  std::filesystem::create_directory(output_dir);

  auto p = Jsonreader::setup_problem(topology, boundary, output_dir);

  int number = p->set_indices();
  std::cout << "Number of variables: " <<number << std::endl;
  try {
    Aux::Printguard guard(p);

    Solver::Newtonsolver solver(1e-8, 50);
    double T = 3600 * 24;

    double N = 48.0;
    double delta_t = T / N;
    Eigen::VectorXd state1(number);

    double new_time(0.0);
    double last_time(0.0);

    

    Jsonreader::set_initial_values(state1, initial, p);
    Eigen::VectorXd state2 = state1;
    p->save_values(0.0,state1);
    solver.evaluate_state_derivative_triplets(*p,
                                               last_time,  new_time,
                                              state2,
                                              state1);
    std::cout << "data read" << std::endl;
    for (int i = 1; i != N + 1; ++i) {
      new_time = i * delta_t;
      auto solstruct = solver.solve(state1, *p, false, last_time, new_time, state2);
      p->save_values(new_time, state1);

      std::cout << new_time << ": ";
      std::cout << solstruct.residual << ", ";
      std::cout << solstruct.used_iterations << std::endl;

      // write new_state to last state:
      state2 = state1;
      // set next time step:
      last_time = new_time;
    }

    p->print_to_files();
  } catch (...) {
    std::cout << "An uncaught exception was thrown!\n"
              << "All available data has been printed to output files.\n"
              << "\nUse with caution!\n"
              << std::endl;
    throw;
  }
}
