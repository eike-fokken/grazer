
#include <Aux_executable.hpp>
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

  char** cmd_argument_pointer = argv;
  std::vector<std::string> cmd_arguments(cmd_argument_pointer+1,
                                         cmd_argument_pointer + argc);

  auto [topology, initial, boundary, delta_t, delta_x,T] = Aux_executable::extract_input_data(cmd_arguments);

  std::filesystem::path output_dir =  Aux_executable::prepare_output_dir("output");

  auto p = Jsonreader::setup_problem(topology, boundary, output_dir,delta_x);

  int number = p->set_indices();
  std::cout << "Number of variables: " <<number << std::endl;
  // p->display();
  try {
    Aux::Printguard guard(p);

    Solver::Newtonsolver solver(1e-8, 50);

    int N = static_cast<int>(std::ceil(T / delta_t));
    double delta_t = (T / N);

    Eigen::VectorXd state1(number);

    
    double last_time(-delta_t);
    double new_time=0.0;
    Jsonreader::set_initial_values(state1, initial, p);
    Eigen::VectorXd state2 = state1;
    // p->save_values(0.0,state1);
    solver.evaluate_state_derivative_triplets(*p,
                                               last_time,  new_time,
                                              state2,
                                              state1);
    std::cout << "number of non-zeros in jacobian: " << solver.get_number_non_zeros_jacobian() << std::endl;
    std::cout << "data read" << std::endl;

    for (int i = 0; i != N + 1; ++i) {
      new_time = i * delta_t;
      // maybe always recompute the structure. Its safer and only introduces one analyzePattern per time step!
      auto solstruct = solver.solve(state1, *p, true, last_time, new_time, state2);
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
