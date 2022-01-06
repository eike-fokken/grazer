#include "commands.hpp"
#include "Adaptor.hpp"
#include "Aux_json.hpp"
#include "ControlStateCache.hpp"
#include "Exception.hpp"
#include "Full_factory.hpp"
#include "ImplicitOptimizer.hpp"
#include "Input_output.hpp"
#include "InterpolatingVector.hpp"
#include "Misc.hpp"
#include "Netfactory.hpp"
#include "Networkproblem.hpp"
#include "Optimization_helpers.hpp"
#include "Timeevolver.hpp"
#include "helpers.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>

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
    auto simulation_settings = all_json["simulation_settings"];
    auto problem_json = all_json["problem_data"];
    // give the path information of the file:
    problem_json["GRAZER_file_directory"] = problem_directory.string();
    auto initial_json = aux_json::get_json_from_file_path(
        problem_directory / std::filesystem::path("initial.json"));
    Model::Timedata timedata(simulation_settings);
    auto timeevolver_ptr
        = Model::Timeevolver::make_pointer_instance(simulation_settings);
    auto &timeevolver = *timeevolver_ptr;

    Model::Componentfactory::Full_factory componentfactory(
        problem_json.value("defaults", R"({})"_json));
    auto net_ptr = Model::build_net(problem_json, componentfactory);
    auto problem_ptr
        = std::make_unique<Model::Networkproblem>(std::move(net_ptr));
    auto &problem = *problem_ptr;
    problem.init();

    Eigen::VectorXd initial_state(problem.get_number_of_states());
    problem.set_initial_values(initial_state, initial_json);

    auto states_timehelper = Aux::make_from_start_delta_end(
        timedata.get_starttime(), timedata.get_delta_t(),
        timedata.get_endtime());
    Aux::InterpolatingVector saved_states(
        states_timehelper, problem.get_number_of_states());
    Eigen::VectorXd state_timepoints = saved_states.get_interpolation_points();
    // set controls, if any:
    Aux::InterpolatingVector full_controls;
    setup_controls(
        full_controls, problem, timedata, all_json, problem_directory);

    std::cout << "Number of control discretization points: "
              << full_controls.size() << std::endl;

    std::cout << "data read" << std::endl;
    wall_clock_setup_end = Clock::now();

    bool optimize = false;
    if (all_json.contains("optimize")) {
      if (all_json["optimize"].is_boolean()) {
        optimize = all_json["optimize"];
      }
    }
    if (not optimize) {
      auto problem_lifetime_manager = std::move(problem_ptr);
      auto timeevolver_lifetime_manager = std::move(timeevolver_ptr);
      timeevolver.simulate(initial_state, full_controls, problem, saved_states);

      for (Eigen::Index index = 0; index != saved_states.size(); ++index) {
        problem.json_save(
            saved_states.interpolation_point_at_index(index),
            saved_states.vector_at_index(index));
      }
      // output to json:
      try {
        // add_results_to_json();
        problem.add_results_to_json(output_json);
        std::ofstream o(output_file);
        o << output_json.dump(1, '\t');
      } catch (std::exception &e) {
        std::ostringstream o;
        o << "Printing to files failed with error message:"
          << "\n###############################################\n"
          << e.what()
          << "\n###############################################\n\n";
        throw std::runtime_error(o.str());
      }
      wall_clock_sim_end = Clock::now();
    } else {
      // optimize!
      Eigen::Index n = 1;
      if (all_json.contains("constraint_settings")) {
        auto &constraint_settings = all_json["constraint_settings"];
        if (constraint_settings.contains("evaluate_constraints_every_n")
            and constraint_settings["evaluate_constraints_every_n"]
                    .is_number_integer()) {
          n = constraint_settings["evaluate_constraints_every_n"];
        }
      }

      if (n <= 0) {
        std::cout << "\"evaluate_constraints_every_n \" was negative, is now "
                     "set to one!";
        n = 1;
      }
      // compute constraint_times:
      Eigen::Index number_of_constrainttimes = -1;
      if ((state_timepoints.size() - 1) % n == 0) {
        number_of_constrainttimes = (state_timepoints.size() - 1) / n;
      } else {
        number_of_constrainttimes = (state_timepoints.size() - 1) / n + 1;
      }
      Eigen::VectorXd constraint_timepoints(number_of_constrainttimes);
      for (Eigen::Index index = 0; index < constraint_timepoints.size();
           ++index) {

        constraint_timepoints[index] = state_timepoints[1 + n * index];
      }
      if (state_timepoints.size() % n != 0) {
        back(constraint_timepoints) = back(state_timepoints);
      }

      auto lower_bounds_json = aux_json::get_json_from_file_path(
          problem_directory / std::filesystem::path("lower_bounds.json"));
      auto upper_bounds_json = aux_json::get_json_from_file_path(
          problem_directory / std::filesystem::path("upper_bounds.json"));
      auto constraint_lower_bounds_json = aux_json::get_json_from_file_path(
          problem_directory
          / std::filesystem::path("constraint_lower_bounds.json"));
      auto constraint_upper_bounds_json = aux_json::get_json_from_file_path(
          problem_directory
          / std::filesystem::path("constraint_upper_bounds.json"));

      Eigen::VectorXd control_timepoints
          = full_controls.get_interpolation_points();
      Aux::InterpolatingVector lower_bounds(
          control_timepoints, problem.get_number_of_controls_per_timepoint());
      Aux::InterpolatingVector upper_bounds(
          control_timepoints, problem.get_number_of_controls_per_timepoint());
      Aux::InterpolatingVector constraint_lower_bounds(
          constraint_timepoints,
          problem.get_number_of_constraints_per_timepoint());
      Aux::InterpolatingVector constraint_upper_bounds(
          constraint_timepoints,
          problem.get_number_of_constraints_per_timepoint());

      Optimization::initialize_bounds(
          problem, lower_bounds, lower_bounds_json, upper_bounds,
          upper_bounds_json, constraint_lower_bounds,
          constraint_lower_bounds_json, constraint_upper_bounds,
          constraint_upper_bounds_json);

      auto cache_ptr = std::make_unique<Optimization::ControlStateCache>(
          std::move(timeevolver_ptr));
      auto optimizer_ptr = std::make_unique<Optimization::ImplicitOptimizer>(
          std::move(problem_ptr), std::move(cache_ptr), state_timepoints,
          control_timepoints, constraint_timepoints, initial_state,
          full_controls, lower_bounds, upper_bounds, constraint_lower_bounds,
          constraint_upper_bounds);
      auto &optimizer = *optimizer_ptr;
      Optimization::IpoptAdaptor adaptor(std::move(optimizer_ptr));
      std::cout << optimizer.get_initial_controls() << std::endl;
      adaptor.optimize();
      std::cout << "solution: " << adaptor.get_solution() << std::endl;
      std::cout << "objective: " << adaptor.get_obj_value() << std::endl;
      wall_clock_sim_end = Clock::now();
    }

    /* ----------------------------------------------------------------- */
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
