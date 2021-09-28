#include "Equationcomponent.hpp"
#include "Exception.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <map>

namespace Model::Networkproblem {

  void Equationcomponent::prepare_timestep(
      double /*last_time*/, double /*new_time*/,
      Eigen::Ref<Eigen::VectorXd const> /*last_state*/,
      Eigen::Ref<Eigen::VectorXd const> /*new_state*/,
      Eigen::Ref<Eigen::VectorXd const> /*last_control*/,
      Eigen::Ref<Eigen::VectorXd const> /*new_control*/) {}

  void Equationcomponent::setup() {}

  std::optional<nlohmann::json> Equationcomponent::get_boundary_schema() {
    return std::nullopt;
  }

  std::optional<nlohmann::json> Equationcomponent::get_control_schema() {
    return std::nullopt;
  }

} // namespace Model::Networkproblem
