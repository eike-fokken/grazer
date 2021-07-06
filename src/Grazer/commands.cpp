#include "commands.hpp"
#include "Aux_json.hpp"
#include "Input_output.hpp"
#include "Problem.hpp"
#include "Timeevolver.hpp"
#include <cstdlib>
#include <iostream>

int grazer::run(std::filesystem::path directory_path) {
  auto problem_directory = directory_path / "problem";
  auto problem_data_file = problem_directory / "problem_data.json";
  auto output_dir = io::prepare_output_dir(directory_path / "output");

  // This must be wrapped in exception handling code!

  auto all_json = aux_json::get_json_from_file_path(problem_data_file);

  auto time_evolution_json = all_json["time_evolution_data"];
  auto problem_json = all_json["problem_data"];

  // give the path information of the file:
  problem_json["GRAZER_file_directory"] = problem_directory.string();

  auto initial_value_json = all_json["initial_values"];
  auto control_value_json = all_json["control_values"];
  initial_value_json["GRAZER_file_directory"] = problem_directory.string();
  control_value_json["GRAZER_file_directory"] = problem_directory.string();
  Model::Timedata timedata(time_evolution_json);

  auto timeevolver = Model::Timeevolver::make_instance(time_evolution_json);

  Model::Problem problem(problem_json, output_dir);
  int number_of_states = problem.set_state_indices();
  int number_of_controls = problem.set_control_indices();
  std::cout << "data read" << std::endl;

  timeevolver.simulate(
      timedata, problem, number_of_states, initial_value_json // ,
      // number_of_controls, control_value_json
  );
  return EXIT_SUCCESS;
}
