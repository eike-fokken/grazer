#include "Controlhelpers.hpp"
#include "Exception.hpp"

namespace Aux {

  Controller::Controller(
      int _number_of_controls_per_timepoint, int _number_of_timesteps) :
      allcontrols(_number_of_controls_per_timepoint * _number_of_timesteps),
      number_of_controls_per_timepoint(_number_of_controls_per_timepoint) {}

  void Controller::set_controls(Eigen::Ref<Eigen::VectorXd> values) {
    if (values.size() != allcontrols.size()) {
      gthrow({"You are trying to assign the wrong number of controls."});
    }
    allcontrols = values;
  }

  Eigen::Index Controller::get_number_of_controls() const {
    return allcontrols.size();
  }

  Eigen::Ref<Eigen::VectorXd>
  Controller::operator()(int current_timestep) const {
    auto start_index = number_of_controls_per_timepoint * current_timestep;
    auto after_index = start_index + number_of_controls_per_timepoint;
    if (after_index > allcontrols.size()) {
      gthrow({"You request controls at a later time than possible!"});
    }
    return allcontrols.segment(start_index, number_of_controls_per_timepoint);
  }
} // namespace Aux
