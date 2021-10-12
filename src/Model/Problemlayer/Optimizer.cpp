#include "Optimizer.hpp"
#include "Networkproblem.hpp"
#include "Timeevolver.hpp"
#include "Wrapper.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

namespace Model {

  nlohmann::json Optimizer::get_schema() {
    nlohmann::json schema;
    schema["type"] = "object";
    return schema;
  }

  Optimizer::Optimizer(
      nlohmann::json const &optimization_json,
      nlohmann::json const &time_evolution_json) :
      timeevolver(Timeevolver::make_instance(time_evolution_json)) {}

  Optimizer Optimizer::make_instance(
      nlohmann::json const &optimization_json,
      nlohmann::json const &time_evolution_json) {
    Aux::schema::validate_json(optimization_json, get_schema());
    return Optimizer(optimization_json, time_evolution_json);
  }

  void Optimizer::optimize(
      Timedata timedata, Networkproblem::Networkproblem &problem,
      nlohmann::json &problem_initial_state_json,
      nlohmann::json &problem_initial_control_json) {
    double last_time = timedata.get_starttime();

    auto number_of_states = problem.get_number_of_states();
    auto number_of_controls_per_time_point
        = problem.get_number_of_controls_per_timepoint();
    auto number_of_inequalities_per_time_point
        = problem.get_number_of_inequalities_per_timepoint();

    Eigen::VectorXd control(
        number_of_controls_per_time_point * timedata.get_number_of_steps());
    Eigen::VectorXd lower_bounds(
        number_of_controls_per_time_point * timedata.get_number_of_steps());
    Eigen::VectorXd upper_bounds(
        number_of_controls_per_time_point * timedata.get_number_of_steps());

    Eigen::VectorXd inequalities(
        number_of_inequalities_per_time_point * timedata.get_number_of_steps());
    Eigen::VectorXd inequalities_lower_bounds(
        number_of_inequalities_per_time_point * timedata.get_number_of_steps());
    Eigen::VectorXd inequalities_upper_bounds(
        number_of_inequalities_per_time_point * timedata.get_number_of_steps());

    // problem.set_initial_values(last_state, problem_initial_state_json);
    // problem.json_save(last_time, last_state);

    for (int i = 0; i != timedata.get_number_of_steps(); ++i) {
      double new_time = last_time + timedata.get_delta_t();
    }
  }

} // namespace Model
