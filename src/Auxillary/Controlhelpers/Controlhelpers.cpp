#include "Controlhelpers.hpp"
#include "Exception.hpp"

namespace Aux {

  Controller::Controller(
      int _number_of_controls_per_timepoint, int _number_of_timesteps) :
      number_of_controls_per_timepoint(_number_of_controls_per_timepoint) {
    if (number_of_controls_per_timepoint < 0) {
      gthrow({"You can't have less than 0 controls per time step!"});
    }
    if (_number_of_timesteps < 1) {
      gthrow({"You can't have less than 1 time step!"});
    }
    allcontrols = Eigen::VectorXd(
        _number_of_controls_per_timepoint * _number_of_timesteps);
  }

  void Controller::set_controls(Eigen::Ref<Eigen::VectorXd> values) {
    if (values.size() != allcontrols.size()) {
      gthrow({"You are trying to assign the wrong number of controls."});
    }
    allcontrols = values;
  }

  Eigen::Index Controller::get_number_of_controls() const {
    return allcontrols.size();
  }

  Eigen::Ref<Eigen::VectorXd const> const
  Controller::operator()(int current_timestep) const {
    if (current_timestep < 0) {
      gthrow({"You request controls at negative time steps!"});
    }
    auto start_index = number_of_controls_per_timepoint * current_timestep;
    auto after_index = start_index + number_of_controls_per_timepoint;
    if (after_index > allcontrols.size()) {
      gthrow({"You request controls at a higher index than possible!"});
    }
    return allcontrols.segment(start_index, number_of_controls_per_timepoint);
  }

  Eigen::Ref<Eigen::VectorXd const> const Controller::get_allcontrols() const {
    return allcontrols;
  }

  Eigen::Ref<Eigen::VectorXd> Controller::mut_timestep(int current_timestep) {
    if (current_timestep < 0) {
      gthrow({"You request controls at negative time steps!"});
    }
    auto start_index = number_of_controls_per_timepoint * current_timestep;
    auto after_index = start_index + number_of_controls_per_timepoint;
    if (after_index > allcontrols.size()) {
      gthrow({"You request controls at a higher index than possible!"});
    }
    return allcontrols.segment(start_index, number_of_controls_per_timepoint);
  }

} // namespace Aux
