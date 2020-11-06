#include <Vphinode.hpp>

namespace Model::Networkproblem::Power {


   // Careful: new state and old state and function value needed!
  //////
  //////
  void Model::Networkproblem::Power::Vphinode::evaluate(Eigen::VectorXd & rootfunction,
    double last_time, double new_time, const Eigen::VectorXd &last_state,
    Eigen::VectorXd &new_state) {
  auto index = get_start_state_index();
  rootfunction[index] = boundaryvalue(new_time)[0] - new_state[index];
  index++;
  rootfunction[index] = boundaryvalue(new_time)[1] - new_state[index];
}

} // namespace Model::Networkproblem::Power
