#include "commands.hpp"
#include "Aux_json.hpp"
#include "Input_output.hpp"
#include "Problem.hpp"
#include "Timeevolver.hpp"
#include <chrono>
#include <cstdlib>
#include <iostream>

int grazer::run(std::filesystem::path directory_path) {
  using Clock = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<Clock>;

  TimePoint wall_clock_start = Clock::now();
  TimePoint wall_clock_setup_end;
  TimePoint wall_clock_sim_end;

  {
    auto problem_directory = directory_path / "problem";
    auto problem_data_file = problem_directory / "problem_data.json";
    auto output_file = io::prepare_output_file(directory_path);

    std::cout << "Using output filename\n" << output_file.string() << std::endl;

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

    Model::Problem problem(problem_json, output_file);
    auto number_of_states = problem.set_state_indices();
  auto number_of_controls = problem.set_control_indices();
    std::cout << "data read" << std::endl;

    wall_clock_setup_end = Clock::now();

    // ---------------- actual simulation ------------------------------ //
    timeevolver.simulate(
        timedata, problem, number_of_states, initial_value_json // ,
      // number_of_controls, control_value_json
  );
    // ----------------------------------------------------------------- //

    wall_clock_sim_end = Clock::now();
  }

  TimePoint wall_clock_end = Clock::now();

  auto duration_in_seconds_with_mus_accuracy
      = [](Clock::duration dur) -> double {
    using Mus = std::chrono::microseconds;
    auto msdur = std::chrono::duration_cast<Mus>(dur).count();
    auto double_msdur = static_cast<double>(msdur);
    return double_msdur / 1e6;
  };

  auto setup_duration = duration_in_seconds_with_mus_accuracy(
      wall_clock_setup_end - wall_clock_start);
  auto sim_duration = duration_in_seconds_with_mus_accuracy(
      wall_clock_sim_end - wall_clock_setup_end);
  auto teardown_duration = duration_in_seconds_with_mus_accuracy(
      wall_clock_end - wall_clock_sim_end);
  auto total_duration = duration_in_seconds_with_mus_accuracy(
      wall_clock_end - wall_clock_start);

  std::cout << "setup took:      " << setup_duration << " seconds" << std::endl;
  std::cout << "simulation took: " << sim_duration << " seconds" << std::endl;
  std::cout << "teardown took:   " << teardown_duration << " seconds"
            << std::endl;
  std::cout << "-----------------------------" << std::endl;
  std::cout << "total:           " << total_duration << " seconds" << std::endl;
  return EXIT_SUCCESS;
}
