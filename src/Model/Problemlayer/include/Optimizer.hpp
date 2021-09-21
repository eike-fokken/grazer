#include "Timeevolver.hpp"
#include <nlohmann/json.hpp>

namespace Model {

  class Optimizer {

  public:
    static nlohmann::json get_schema();
    static Optimizer make_instance(nlohmann::json const &optimizer_data);

    void simulate(
        Timedata timedata, Problem &problem, int number_of_states,
        nlohmann::json &problem_initial_json);

  private:
    Optimizer(nlohmann::json const &optimizer_data);
    Solver::Newtonsolver<Problem> solver;
    int retries;
    bool const use_simplified_newton;
  };
} // namespace Model
