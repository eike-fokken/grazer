#include "nlohmann/json.hpp"
#include <Eigen/Sparse>
#include <Newtonsolver.hpp>
#include <memory>
#include <utility>

// Forward declarations:
namespace Model {
  class Problem;
}

namespace Model {
  using json = nlohmann::ordered_json;

  struct Timedata{

    Timedata() = delete;
    Timedata(json timedata_json);

    double const starttime;
    double const endtime;
    double const Full_time_interval;
    int const Number_of_timesteps;

    // maybe later on this should be made non-const for variable stepsizes:
    double const delta_t;

  private:
    int init_Number_of_timesteps(double desired_delta_t) const;

    double init_delta_t() const;
  };


  class Timeevolver {

  public:
    Timeevolver() = delete;
    Timeevolver(double tolerance, int maximal_number_of_iterations);

    void simulate(Timedata timedata, Model::Problem & problem,
                  int number_of_states, json problem_initial_json);

  private :
        Solver::Newtonsolver solver;
    };

} // namespace Model
