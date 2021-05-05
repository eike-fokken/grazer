#include "Timeevolver.hpp"
#include "Problem.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace Model {

  Timedata::Timedata(nlohmann::json const &time_evolution_data) :
      starttime(time_evolution_data["start_time"].get<double>()),
      endtime(time_evolution_data["end_time"].get<double>()),
      Number_of_timesteps(init_Number_of_timesteps(
          time_evolution_data["desired_delta_t"].get<double>())),
      delta_t(init_delta_t()) {
    std::cout << "Start_time: " << starttime << std::endl;
    std::cout << "end_time: " << endtime << std::endl;
    std::cout << "delta_t: " << delta_t << std::endl;
  }

  int Timedata::init_Number_of_timesteps(double desired_delta_t) const {
    int const _Number_of_timesteps
        = static_cast<int>(std::ceil(get_timeinterval() / desired_delta_t));
    return _Number_of_timesteps;
  }

  double Timedata::get_starttime() const { return starttime; }
  double Timedata::get_endtime() const { return endtime; }
  double Timedata::get_delta_t() const { return delta_t; }
  double Timedata::get_timeinterval() const { return endtime - starttime; }
  int Timedata::get_number_of_steps() const { return Number_of_timesteps; }

  double Timedata::init_delta_t() const {
    return get_timeinterval() / Number_of_timesteps;
  }

  Timeevolver::Timeevolver(
      double tolerance, int maximal_number_of_iterations,
      std::filesystem::path const &_output_dir) :
      solver(tolerance, maximal_number_of_iterations),
      output_dir(_output_dir) {}

  void Timeevolver::simulate(
      Timedata timedata, Model::Problem &problem, int number_of_states,
      nlohmann::json &problem_initial_json) {
    double last_time = timedata.get_starttime();
    Eigen::VectorXd last_state(number_of_states);

    problem.set_initial_values(last_state, problem_initial_json);

    // This initializes P and Q-values of P-Q-nodes.
    problem.prepare_timestep(last_time, last_time, last_state, last_state);
    // save the initial values.
    problem.json_save(last_time, last_state);

    double new_time = last_time + timedata.get_delta_t();

    Eigen::VectorXd new_state = last_state;

    solver.evaluate_state_derivative_triplets(
        problem, last_time, new_time, last_state, new_state);

    std::cout << "Number of variables: " << number_of_states << std::endl;
    std::cout << "number of non-zeros in jacobian: "
              << solver.get_number_non_zeros_jacobian() << std::endl;

    int retry = 0;

    for (int i = 0; i != timedata.get_number_of_steps(); ++i) {
      new_time = last_time + timedata.get_delta_t();
      problem.prepare_timestep(last_time, new_time, last_state, new_state);
      auto solstruct = solver.solve(
          new_state, problem, true, last_time, new_time, last_state);
      std::cout << new_time << ": ";
      std::cout << solstruct.residual << ", ";
      std::cout << solstruct.used_iterations << std::endl;
      if (solstruct.success) {
        problem.json_save(new_time, new_state);
        retry = 0;
      } else {
        if (retry < 2) {
          --i;
          ++retry;
          std::cout << "\nretrying timestep " << new_time << "\n" << std::endl;
          continue;
        } else {
          std::cout << "Failed timestep irrevocably!" << new_time << std::endl;
          problem.json_save(new_time, new_state);
        }
      }
      last_state = new_state;
      last_time = new_time;
    }
  }

  std::filesystem::path const &Timeevolver::get_output_dir() {
    return output_dir;
  }

} // namespace Model
