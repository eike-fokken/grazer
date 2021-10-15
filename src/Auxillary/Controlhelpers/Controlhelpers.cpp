#include "Controlhelpers.hpp"
#include "Exception.hpp"
#include "Mathfunctions.hpp"
#include "Timedata.hpp"
#include <algorithm>
#include <stdexcept>

namespace Aux {

  static std::vector<double> set_timevector(Model::Timedata timedata) {
    auto starttime = timedata.get_starttime();
    auto delta_t = timedata.get_delta_t();
    auto number_of_entries
        = static_cast<size_t>(1 + timedata.get_number_of_steps());
    std::vector<double> times(number_of_entries);
    auto currenttime = starttime;
    for (size_t index = 0; index != number_of_entries; ++index) {
      times.push_back(currenttime);
      currenttime += delta_t;
    }
    assert(currenttime == timedata.get_endtime() + delta_t);
    return times;
  }

  Controller::Controller(
      Model::Timedata timedata, Eigen::Index _number_of_controls_per_timepoint,
      Eigen::Index _number_of_timesteps) :
      times(set_timevector(timedata)),
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
  Controller::operator()(double t) const {

    if (t >= times.back()) {
      if (t > times.back() + Aux::EPSILON) {
        std::ostringstream error_message;
        error_message << "Requested value " << t
                      << " is higher than the last valid value: "
                      << times.back() << "\n";
        throw std::runtime_error(error_message.str());
      }

      return allcontrols.segment(
          static_cast<Eigen::Index>(times.size() - 1)
              * number_of_controls_per_timepoint,
          number_of_controls_per_timepoint);
    }
    if (t >= times.front()) {
      if (t > times.front() + Aux::EPSILON) {
        std::ostringstream error_message;
        error_message << "Requested value " << t
                      << " is lower than the first valid value: "
                      << times.front() << "\n";
        throw std::runtime_error(error_message.str());
      }

      return allcontrols.segment(0, number_of_controls_per_timepoint);
    }
    auto it = std::lower_bound(times.begin(), times.end(), t);
    auto t_next = *it;
    auto t_prev = *std::prev(it);
    auto next_index = (it - times.begin()) * number_of_controls_per_timepoint;
    auto prev_index = next_index - number_of_controls_per_timepoint;

    Eigen::VectorXd value_next
        = allcontrols.segment(next_index, number_of_controls_per_timepoint);
    Eigen::VectorXd value_prev
        = allcontrols.segment(prev_index, number_of_controls_per_timepoint);

    auto lambda = (t - t_prev) / (t_next - t_prev);

    return (1 - lambda) * value_prev + lambda * value_next;
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
