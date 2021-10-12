#pragma once
#include "Newtonsolver.hpp"
#include <nlohmann/json.hpp>

namespace Model {
  namespace Networkproblem {
    class Networkproblem;
  }

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
    static nlohmann::json get_schema();
    static Timeevolver make_instance(nlohmann::json const &timeevolver_data);

    void simulate(
        Timedata timedata, Networkproblem::Networkproblem &problem,
        nlohmann::json &problem_initial_json);

    Solver::Solutionstruct make_one_step(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd> last_state,
        Eigen::Ref<Eigen::VectorXd> new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control,
        Networkproblem::Networkproblem &problem);

  private:
    Timeevolver(nlohmann::json const &timeevolver_data);
    Solver::Newtonsolver<Networkproblem::Networkproblem> solver;
    int retries;
    bool const use_simplified_newton;
  };

} // namespace Model
