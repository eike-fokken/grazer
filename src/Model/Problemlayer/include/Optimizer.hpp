#pragma once
#include "Timeevolver.hpp"
#include <nlohmann/json.hpp>

namespace Model {

  class Optimizer {

  public:
    static nlohmann::json get_schema();
    static Optimizer make_instance(
        nlohmann::json const &optimization_json,
        nlohmann::json const &time_evolution_json);

    void optimize(
        Timedata timedata, Networkproblem::Networkproblem &problem,
        nlohmann::json &problem_initial_state_json,
        nlohmann::json &problem_initial_control_json);

  private:
    Optimizer(
        nlohmann::json const &optimization_json,
        nlohmann::json const &time_evolution_json);
    Timeevolver timeevolver;
  };
} // namespace Model
