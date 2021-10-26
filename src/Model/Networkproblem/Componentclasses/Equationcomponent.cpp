#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <map>

namespace Model {

  void Equationcomponent::prepare_timestep(
      double /*last_time*/, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) {}

} // namespace Model
