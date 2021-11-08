#pragma once
#include "Newtonsolver.hpp"
#include "Timedata.hpp"
#include <nlohmann/json.hpp>

namespace Aux {
  class InterpolatingVector;
}

namespace Model {

  class Controlcomponent;

  class Timeevolver {

  public:
    static nlohmann::json get_schema();
    static Timeevolver make_instance(nlohmann::json const &timeevolver_data);

    void simulate(
        Eigen::Ref<Eigen::VectorXd const> const &initial_state,
        Aux::InterpolatingVector const &controls,
        Controlcomponent &problem, Aux::InterpolatingVector &saved_states);

    Solver::Solutionstruct make_one_step(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd> last_state,
        Eigen::Ref<Eigen::VectorXd> new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control,
        Controlcomponent &problem);

  private:
    Timeevolver(nlohmann::json const &timeevolver_data);

    Solver::Newtonsolver solver;
    int const retries;
    bool const use_simplified_newton;
  };

} // namespace Model
