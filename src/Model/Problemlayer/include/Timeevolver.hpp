#pragma once
#include "Newtonsolver.hpp"
#include "Problem.hpp"
#include <Eigen/Sparse>
#include <memory>
#include <nlohmann/json.hpp>
#include <utility>

namespace Model {

  struct Timedata {

    Timedata() = delete;
    Timedata(nlohmann::json const &time_evolution_data);

    double get_starttime() const;
    double get_endtime() const;
    double get_delta_t() const;
    double get_timeinterval() const;
    int get_number_of_steps() const;

  private:
    double const starttime;
    double const endtime;
    int const Number_of_timesteps;

    // maybe later on this should be made non-const for variable stepsizes:
    double const delta_t;

    int init_Number_of_timesteps(double desired_delta_t) const;

    double init_delta_t() const;
  };

  class Timeevolver {

  public:
    Timeevolver() = delete;
    Timeevolver(double tolerance, int maximal_number_of_iterations);

    void simulate(
        Timedata timedata, Model::Problem &problem, int number_of_states,
        nlohmann::json &problem_initial_json);

  private:
    Solver::Newtonsolver<Problem> solver;
  };

} // namespace Model
