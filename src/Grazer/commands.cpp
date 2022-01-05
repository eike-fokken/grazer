#include "commands.hpp"
#include "Aux_json.hpp"
#include "Exception.hpp"
#include "Full_factory.hpp"
#include "Input_output.hpp"
#include "InterpolatingVector.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "Optimization_helpers.hpp"
#include "Timeevolver.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

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
    nlohmann::json output_json;
    std::cout << "Using output filename\n" << output_file.string() << std::endl;

    // This must be wrapped in exception handling code!

    auto all_json = aux_json::get_json_from_file_path(problem_data_file);
    auto time_evolution_json = all_json["simulation_settings"];
    auto problem_json = all_json["problem_data"];
    // give the path information of the file:
    problem_json["GRAZER_file_directory"] = problem_directory.string();
    auto initial_json = aux_json::get_json_from_file_path(
        problem_directory / std::filesystem::path("initial.json"));
    Model::Timedata timedata(time_evolution_json);
    auto timeevolver = Model::Timeevolver::make_instance(time_evolution_json);

    Model::Componentfactory::Full_factory componentfactory(
        problem_json.value("defaults", R"({})"_json));
    auto net_ptr = Model::build_net(problem_json, componentfactory);
    Model::Networkproblem problem(std::move(net_ptr));
    problem.init();

    Eigen::VectorXd initial_state(problem.get_number_of_states());
    problem.set_initial_values(initial_state, initial_json);

    // set controls, if any:
    Aux::InterpolatingVector controls;
    if (problem.get_number_of_controls_per_timepoint() > 0) {
      auto control_json = aux_json::get_json_from_file_path(
          problem_directory / std::filesystem::path("control.json"));
      auto control_timehelper = Aux::make_from_start_delta_number(
          timedata.get_starttime(), timedata.get_delta_t(),
          timedata.get_number_of_time_points());
      controls = Aux::InterpolatingVector(
          control_timehelper, problem.get_number_of_controls_per_timepoint());
      problem.set_initial_controls(controls, control_json);
    }
    std::cout << "data read" << std::endl;

    wall_clock_setup_end = Clock::now();

    std::vector<double> saved_times;
    auto states_timehelper = Aux::make_from_start_delta_end(
        timedata.get_starttime(), timedata.get_delta_t(),
        timedata.get_endtime());
    Aux::InterpolatingVector saved_states(
        states_timehelper, problem.get_number_of_states());

    timeevolver.simulate(initial_state, controls, problem, saved_states);

    for (Eigen::Index index = 0; index != saved_states.size(); ++index) {
      problem.json_save(
          saved_states.interpolation_point_at_index(index),
          saved_states.vector_at_index(index));
    }

    /* ----------------------------------------------------------------- */

    wall_clock_sim_end = Clock::now();

    // output to json:
    try {
      // add_results_to_json();
      problem.add_results_to_json(output_json);
      std::ofstream o(output_file);
      o << output_json.dump(1, '\t');
    } catch (std::exception &e) {
      std::cout << "Printing to files failed with error message:"
                << "\n###############################################\n"
                << e.what()
                << "\n###############################################\n\n"
                << std::flush;
    }
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
