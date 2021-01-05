#include "Newtonsolver.hpp"
#include "nlohmann/json.hpp"
#include <Problem.hpp>
#include <Timeevolver.hpp>
#include <iostream>

namespace Model {
  
  Timedata::Timedata(double _starttime, double _endtime, double desired_delta_t):
                     starttime(_starttime), endtime(_endtime),
                     Full_time_interval(endtime - starttime),
                     Number_of_timesteps(init_Number_of_timesteps(desired_delta_t)),
                     delta_t(init_delta_t())
                     {}

  int Timedata::init_Number_of_timesteps(double desired_delta_t) const {
    int const _Number_of_timesteps =
        static_cast<int>(std::ceil(Full_time_interval / desired_delta_t));
    return _Number_of_timesteps;
  }

  double Timedata::init_delta_t() const {
    return Full_time_interval / Number_of_timesteps;
  }

  Timeevolver::Timeevolver(double tolerance,
                           int maximal_number_of_iterations)
    : solver(tolerance,maximal_number_of_iterations) {}



  void Timeevolver::simulate(Timedata timedata, Model::Problem & problem,
                             int number_of_states, json problem_initial_json) {
    double last_time = timedata.starttime;
    Eigen::VectorXd last_state(number_of_states);

    problem.set_initial_values(last_state,problem_initial_json);
    problem.save_values(last_time, last_state);

    double new_time = last_time+ timedata.delta_t;

    Eigen::VectorXd new_state(last_state);

    for (int i = 0; i != timedata.Number_of_timesteps; ++i) {
      last_state = new_state;
      new_time = last_time + timedata.delta_t;

      auto solstruct = solver.solve(new_state, problem, true, last_time,
                                    new_time, last_state);
      problem.save_values(new_time, new_state);

      std::cout << new_time << ": ";
      std::cout << solstruct.residual << ", ";
      std::cout << solstruct.used_iterations << std::endl;

      last_time = new_time;

    }
  }

}
