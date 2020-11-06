#include <Vphinode.hpp>

namespace Model::Networkproblem::Power {


   // Careful: new state and old state and function value needed!
  //////
  //////
  void Model::Networkproblem::Power::Vphinode::evaluate(Eigen::VectorXd & rootfunction,
    double current_time, double next_time, const Eigen::VectorXd &last_state,
    Eigen::VectorXd &current_state) {
  auto index = get_start_state_index();
  rootfunction[index] = boundaryvalue(next_time)[0] - current_state[index];
  index++;
  rootfunction[index] = boundaryvalue(next_time)[1] - current_state[index];
}

} // namespace Model::Networkproblem::Power
