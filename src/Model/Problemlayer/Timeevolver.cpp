#include "Timeevolver.hpp"
#include "Controlcomponent.hpp"
#include "Exception.hpp"
#include "InterpolatingVector.hpp"
#include "Newtonsolver.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
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

  std::unique_ptr<Timeevolver>
  Timeevolver::make_pointer_instance(nlohmann::json const &timeevolver_data) {
    Aux::schema::validate_json(timeevolver_data, get_schema());
    return std::unique_ptr<Timeevolver>(new Timeevolver(timeevolver_data));
  }

  Timeevolver::Timeevolver(nlohmann::json const &timeevolver_data) :
      solver(
          timeevolver_data["tolerance"],
          timeevolver_data["maximal_number_of_newton_iterations"]),
      retries(timeevolver_data["retries"]),
      use_simplified_newton(timeevolver_data["use_simplified_newton"]) {}

  void Timeevolver::simulate(
      Eigen::Ref<Eigen::VectorXd const> const &initial_state,
      Aux::InterpolatingVector_Base const &controls, Controlcomponent &problem,
      Aux::InterpolatingVector_Base &saved_states) {
    double last_time = saved_states.interpolation_point_at_index(0);
    double new_time = saved_states.interpolation_point_at_index(1);

    Eigen::VectorXd last_state = initial_state;
    Eigen::VectorXd new_state = last_state;

    saved_states.mut_timestep(0) = initial_state;

    Eigen::VectorXd current_controls;
    bool actual_controls = (controls.get_inner_length() > 0);
    if (actual_controls) {
      current_controls = controls(new_time);
    }

    // Here we set the Jacobian structure, never to be changed again.
    solver.evaluate_state_derivative_triplets(
        problem, last_time, new_time, last_state, new_state, current_controls);

    // // provide regex help (cf. helper_functions/csv_from_log.py)
    // std::cout << "=== simulation start ===" << std::endl;
    // // csv heading:
    // std::cout << "t, residual, used_iterations" << std::endl;

    for (int i = 1; i != saved_states.size(); ++i) {
      new_time = saved_states.interpolation_point_at_index(i);
      if (actual_controls) {
        current_controls = controls(new_time);
      }

      auto solstruct = make_one_step(
          last_time, new_time, last_state, new_state, current_controls,
          problem);
      // std::cout << new_time << ", ";
      // std::cout << solstruct.residual << ", ";
      // std::cout << solstruct.used_iterations << std::endl;
      if (not solstruct.success) {
        gthrow({"Failed timestep irrevocably!", std::to_string(new_time)});
      }
      saved_states.mut_timestep(i) = new_state;
      last_time = new_time;
      last_state = new_state;
    }
    // std::cout << "=== simulation end ===" << std::endl; // provide regex help
  }

  Solver::Solutionstruct Timeevolver::make_one_step(
      double last_time, double new_time, Eigen::Ref<Eigen::VectorXd> last_state,
      Eigen::Ref<Eigen::VectorXd> new_state,
      Eigen::Ref<Eigen::VectorXd const> const &control,
      Controlcomponent &problem) {
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
      problem.prepare_timestep(last_time, new_time, last_state, control);
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
