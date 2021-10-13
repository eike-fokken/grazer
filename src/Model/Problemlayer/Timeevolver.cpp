#include "Timeevolver.hpp"
#include "Exception.hpp"
#include "Networkproblem.hpp"
#include "Newtonsolver_declaration.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace Model {

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
      Timedata timedata, Networkproblem &problem,
      nlohmann::json &problem_initial_json) {
    double last_time = timedata.get_starttime();

    auto number_of_states = problem.get_number_of_states();
    Eigen::VectorXd last_state(number_of_states);

    std::cout
        << "#############################################################"
           "#############\n"
        << "FOLLOWING LINE " << __LINE__ << " IN FILE " << __FILE__
        << "\nEMPTY CONTROLS ARE SET. MUST BE REPLACED BY ACTUAL CONTROLS!\n"
        << "#############################################################"
           "#############\n";
    Eigen::VectorXd last_control;
    Eigen::VectorXd control;

    problem.set_initial_values(last_state, problem_initial_json);
    problem.json_save(last_time, last_state);
    double new_time = last_time + timedata.get_delta_t();

    Eigen::VectorXd new_state = last_state;

    solver.evaluate_state_derivative_triplets(
        problem, last_time, new_time, last_state, new_state, control);

    std::cout << "Number of variables: " << number_of_states << std::endl;
    std::cout << "number of non-zeros in jacobian: "
              << solver.get_number_non_zeros_jacobian() << std::endl;

    // provide regex help (cf. helper_functions/csv_from_log.py)
    std::cout << "=== simulation start ===" << std::endl;
    // csv heading:
    std::cout << "t, residual, used_iterations" << std::endl;

    for (int i = 0; i != timedata.get_number_of_steps(); ++i) {
      new_time = last_time + timedata.get_delta_t();
      auto solstruct = make_one_step(
          last_time, new_time, last_state, new_state, control, problem);
      std::cout << new_time << ", ";
      std::cout << solstruct.residual << ", ";
      std::cout << solstruct.used_iterations << std::endl;
      if (not solstruct.success) {
        gthrow({"Failed timestep irrevocably!", std::to_string(new_time)});
      }
      problem.json_save(new_time, new_state);
      last_time = new_time;
      last_state = new_state;
    }

    std::cout << "=== simulation end ===" << std::endl; // provide regex help
  }

  Solver::Solutionstruct Timeevolver::make_one_step(
      double last_time, double new_time, Eigen::Ref<Eigen::VectorXd> last_state,
      Eigen::Ref<Eigen::VectorXd> new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control,
      Networkproblem &problem) {
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
          last_time, new_time, last_state, new_state, control);
      solstruct = solver.solve(
          new_state, problem, false, use_full_jacobian, last_time, new_time,
          last_state, control);
      if (solstruct.success) {

        if (use_simplified_newton and retry > 0) {
          std::cout << "succeeded after retries!\n" << std::endl;
        } else if (not use_simplified_newton and retry > retries) {
          std::cout << "succeeded after retries!\n" << std::endl;
        }
        // Found a successful solution, so leave the function!
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
    return solstruct;
  }

} // namespace Model
