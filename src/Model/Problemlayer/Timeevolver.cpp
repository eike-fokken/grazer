#include "Timeevolver.hpp"
#include "Exception.hpp"
#include "Problem.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

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
        = static_cast<int>(std::ceil(get_timeinterval() / desired_delta_t)) + 1;
    return _Number_of_timesteps;
  }

  double Timedata::get_starttime() const { return starttime; }
  double Timedata::get_endtime() const { return endtime; }
  double Timedata::get_delta_t() const { return delta_t; }
  double Timedata::get_timeinterval() const { return endtime - starttime; }
  int Timedata::get_number_of_steps() const { return Number_of_timesteps; }

  double Timedata::init_delta_t() const {
    return get_timeinterval() / (Number_of_timesteps - 1);
  }

  nlohmann::json Timeevolver::get_schema() {
    nlohmann::json schema;
    schema["type"] = "object";
    Aux::schema::add_required(schema, "tolerance", Aux::schema::type::number());
    Aux::schema::add_required(
        schema, "maximal_number_of_newton_iterations",
        Aux::schema::type::number());
    Aux::schema::add_required(schema, "retries", Aux::schema::type::number());
    Aux::schema::add_required(
        schema, "use_simplified_newton", Aux::schema::type::boolean());

    return schema;
  }

  Timeevolver
  Timeevolver::make_instance(nlohmann::json const &timeevolver_data) {
    Aux::schema::validate_json(timeevolver_data, get_schema());
    return Timeevolver(timeevolver_data);
  }

  Timeevolver::Timeevolver(nlohmann::json const &timeevolver_data) :
      solver(
          timeevolver_data["tolerance"],
          timeevolver_data["maximal_number_of_newton_iterations"]),
      retries(timeevolver_data["retries"]),
      use_simplified_newton(timeevolver_data["use_simplified_newton"]) {}

  void Timeevolver::simulate(
      Timedata timedata, Model::Problem &problem, int number_of_states,
      nlohmann::json &problem_initial_json) {
    double last_time = timedata.get_starttime() - timedata.get_delta_t();
    Eigen::VectorXd last_state(number_of_states);

    std::cout
        << "#############################################################"
           "#############\n"
        << "FOLLOWING LINE " << __LINE__ << " IN FILE " << __FILE__
        << "\nEMPTY CONTROLS ARE SET. MUST BE REPLACED BY ACTUAL CONTROLS!\n"
        << "#############################################################"
           "#############\n";
    Eigen::VectorXd last_control;
    Eigen::VectorXd new_control;

    problem.set_initial_values(last_state, problem_initial_json);

    double new_time = last_time + timedata.get_delta_t();

    Eigen::VectorXd new_state = last_state;

    solver.evaluate_state_derivative_triplets(
        problem, last_time, new_time, last_state, new_state, last_control,
        new_control);

    std::cout << "Number of variables: " << number_of_states << std::endl;
    std::cout << "number of non-zeros in jacobian: "
              << solver.get_number_non_zeros_jacobian() << std::endl;

    // provide regex help (cf. helper_functions/csv_from_log.py)
    std::cout << "=== simulation start ===" << std::endl;
    // csv heading:
    std::cout << "t, residual, used_iterations" << std::endl;

    for (int i = 0; i != timedata.get_number_of_steps(); ++i) {
      new_time = last_time + timedata.get_delta_t();
      Solver::Solutionstruct solstruct;
      int retry = 0;
      if (use_simplified_newton) {
        retry = 0;
      } else {
        retry = retries;
      }
      bool use_full_jacobian = true;
      if (use_simplified_newton) {
        use_full_jacobian = false;
      } else {
        use_full_jacobian = true;
      }
      Eigen::VectorXd new_state_backup = new_state;
      while (not solstruct.success) {
        new_state = new_state_backup;
        problem.prepare_timestep(
            last_time, new_time, last_state, new_state, last_control,
            new_control);
        solstruct = solver.solve(
            new_state, problem, false, use_full_jacobian, last_time, new_time,
            last_state, last_control, new_control);
        if (solstruct.success) {

          if (use_simplified_newton and retry > 0) {
            std::cout << "succeeded after retries!\n" << std::endl;
          } else if (not use_simplified_newton and retry > retries) {
            std::cout << "succeeded after retries!\n" << std::endl;
          }
          problem.json_save(new_time, new_state);
          break;
        }
        if (use_simplified_newton and retry == retries) {
          use_full_jacobian = true;
          std::cout << "Switching to updated Jacobian in every step."
                    << std::endl;
        }
        if (retry > 2 * retries) {
          problem.json_save(new_time, new_state);
          gthrow({"Failed timestep irrevocably!", std::to_string(new_time)});
        }

        ++retry;
        if (use_simplified_newton and retry == 1) {
          std::cout << "\nretrying timestep " << new_time << std::endl;
        } else if (not use_simplified_newton and retry == retries + 1) {
          std::cout << "\nretrying timestep " << new_time << std::endl;
        }
        if (retry > 0) {
          std::cout << new_time << ", ";
          std::cout << solstruct.residual << ", ";
          std::cout << solstruct.used_iterations << std::endl;
        }
      }
      std::cout << new_time << ", ";
      std::cout << solstruct.residual << ", ";
      std::cout << solstruct.used_iterations << std::endl;

      last_state = new_state;
      last_time = new_time;
    }

    std::cout << "=== simulation end ===" << std::endl; // provide regex help
  }

} // namespace Model
