#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <map>

namespace Model::Networkproblem {

  void Equationcomponent::prepare_timestep(
      double // last_time
      ,
      double // new_time
      ,
      Eigen::Ref<Eigen::VectorXd const> // last_state
      ,
      Eigen::Ref<Eigen::VectorXd const> // new_state
  ) {}

  void Equationcomponent::setup() {}

} // namespace Model::Networkproblem
