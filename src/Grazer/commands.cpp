#include "commands.hpp"
#include "Aux_json.hpp"
#include "Exception.hpp"
#include "Full_factory.hpp"
#include "Input_output.hpp"
#include "InterpolatingVector.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "Optimization_helpers.hpp"
#include "Optimizer.hpp"
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

    nlohmann::json optimization_json;
    // Extract the optimization data, if it is there
    if (all_json.contains("optimization_data")) {
      optimization_json = all_json["optimization_data"];
    }
    auto time_evolution_json = all_json["time_evolution_data"];
    auto problem_json = all_json["problem_data"];

    // give the path information of the file:
    problem_json["GRAZER_file_directory"] = problem_directory.string();

    std::cout << "\n" << __FILE__ << ":" << __LINE__ << std::endl;
    std::cout << "FIX FILE HANDLING!!!\n" << std::endl;

    auto initial_json = aux_json::get_json_from_file_path(
        problem_directory / std::filesystem::path("initial.json"));
    auto control_json = aux_json::get_json_from_file_path(
        problem_directory / std::filesystem::path("control.json"));
    // auto lower_bounds_json = aux_json::get_json_from_file_path(
    //     problem_directory / std::filesystem::path("lower_bounds.json"));
    // auto upper_bounds_json = aux_json::get_json_from_file_path(
    //     problem_directory / std::filesystem::path("upper_bounds.json"));
    // auto constraints_lower_bounds_json = aux_json::get_json_from_file_path(
    //     problem_directory
    //     / std::filesystem::path("constraints_lower_bounds.json"));
    // auto constraints_upper_bounds_json = aux_json::get_json_from_file_path(
    //     problem_directory
    //     / std::filesystem::path("constraints_upper_bounds.json"));

    nlohmann::json lower_bounds_json;
    nlohmann::json upper_bounds_json;
    nlohmann::json constraints_lower_bounds_json;
    nlohmann::json constraints_upper_bounds_json;

    nlohmann::json new_control_json;

    Model::Timedata timedata(time_evolution_json);

    auto timeevolver = Model::Timeevolver::make_instance(time_evolution_json);

    Model::Componentfactory::Full_factory componentfactory(
        problem_json.value("defaults", R"({})"_json));
    auto net_ptr = Model::build_net(problem_json, componentfactory);
    Model::Networkproblem problem(std::move(net_ptr));

    problem.init();
    auto control_timehelper = Aux::interpolation_points_helper(
        timedata.get_starttime(), timedata.get_delta_t(),
        timedata.get_number_of_time_points());
    Aux::InterpolatingVector controller(
        control_timehelper, problem.get_number_of_controls_per_timepoint());

    Eigen::VectorXd initial_state(problem.get_number_of_states());
    Eigen::VectorXd lower_bounds(
        problem.get_number_of_controls_per_timepoint()
        * timedata.get_number_of_time_points());
    Eigen::VectorXd upper_bounds(
        problem.get_number_of_controls_per_timepoint()
        * timedata.get_number_of_time_points());
    Eigen::VectorXd constraints_lower_bounds(
        problem.get_number_of_inequalities_per_timepoint()
        * timedata.get_number_of_time_points());
    Eigen::VectorXd constraints_upper_bounds(
        problem.get_number_of_inequalities_per_timepoint()
        * timedata.get_number_of_time_points());

    Optimization::initialize(
        timedata, problem, controller, control_json, initial_state,
        initial_json, lower_bounds, lower_bounds_json, upper_bounds,
        upper_bounds_json, constraints_lower_bounds,
        constraints_lower_bounds_json, constraints_upper_bounds,
        constraints_upper_bounds_json);

    std::cout << "data read" << std::endl;

    wall_clock_setup_end = Clock::now();

    std::vector<double> saved_times;
    std::vector<Eigen::VectorXd> saved_states;

    timeevolver.simulate(
        initial_state, controller, timedata, problem, saved_times,
        saved_states);

    if (saved_times.size() != saved_states.size()) {
      gthrow({"size wrong!"});
    }
    for (std::size_t index = 0; index != saved_times.size(); ++index) {
      problem.json_save(saved_times[index], saved_states[index]);
    }

    // number_of_controls, control_value_json

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
