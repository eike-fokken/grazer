#include "Aux_json.hpp"
#include "Input_output.hpp"
#include "Problem.hpp"
#include "Timeevolver.hpp"
#include <Eigen/Sparse>
#include <any>
#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

int run(
    std::deque<std::string> cmd_arguments,
    std::map<std::string, std::any> kwargs);
int run(
    std::deque<std::string> cmd_arguments,
    std::map<std::string, std::any> /*kwargs*/) {
  std::filesystem::path problem_data_file
      = io::extract_input_data(cmd_arguments);

  std::filesystem::path output_dir = io::prepare_output_dir("output");

  // This must be wrapped in exception handling code!

  auto all_json = aux_json::get_json_from_file_path(problem_data_file);

  auto time_evolution_json = all_json["time_evolution_data"];

  auto problem_json = all_json["problem_data"];

  // give the path information of the file:
  auto directory_path
      = std::filesystem::absolute(problem_data_file).parent_path();
  problem_json["GRAZER_file_directory"] = directory_path.string();

  auto initial_value_json = all_json["initial_values"];
  initial_value_json["GRAZER_file_directory"] = directory_path.string();
  Model::Timedata timedata(time_evolution_json);

  double tolerance = 1e-8;
  int maximal_number_of_newton_iterations = 50;
  Model::Timeevolver timeevolver(
      tolerance, maximal_number_of_newton_iterations, output_dir);

  Model::Problem problem(problem_json, timeevolver.get_output_dir());
  int number_of_states = problem.set_indices();
  std::cout << "data read" << std::endl;

  timeevolver.simulate(timedata, problem, number_of_states, initial_value_json);
  return 0;
}

io::Command grazer(
    {{run, /*options=*/std::vector<io::Option>(), /*arguments=*/{"directory"},
      /*name=*/"run",
      /*description=*/"solve problem"}},
    /*name=*/"grazer",
    /*description=*/"PDE Solver");

int main(int argc, char **argv) {
  return grazer(io::args_as_deque(argc, argv));
}
