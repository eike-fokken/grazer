#pragma once
#include "Newtonsolver.hpp"
#include "Timedata.hpp"
#include <nlohmann/json.hpp>

namespace Aux {
  class InterpolatingVector;
}

namespace Model {

  class Networkproblem;

  class Timeevolver {

  public:
    static nlohmann::json get_schema();
    static Timeevolver make_instance(nlohmann::json const &timeevolver_data);

    void initialize(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> initialstate,
        Aux::InterpolatingVector &controller, Networkproblem &problem,
        nlohmann::json &problem_initial_json, nlohmann::json &control_json);

    void simulate(
        Eigen::Ref<Eigen::VectorXd const> const &initial_state,
        Aux::InterpolatingVector &controller, Timedata timedata,
        Networkproblem &problem, std::vector<double> &saved_times,
        std::vector<Eigen::VectorXd> &saved_states);

    Solver::Solutionstruct make_one_step(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd> last_state,
        Eigen::Ref<Eigen::VectorXd> new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control,
        Networkproblem &problem);

  private:
    Timeevolver(nlohmann::json const &timeevolver_data);
    Solver::Newtonsolver<Networkproblem> solver;
    int retries;
    bool const use_simplified_newton;
  };

} // namespace Model
