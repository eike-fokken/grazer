#include "Controlhelpers.hpp"
#include "Exception.hpp"
#include "Mathfunctions.hpp"
#include "Timedata.hpp"
#include "schema_validation.hpp"
#include <Eigen/src/Core/util/Meta.h>
#include <algorithm>
#include <cstddef>
#include <exception>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace Aux {

  Interpolation_data interpolation_points_helper(
      double first_point, double delta, int number_of_entries) {
    return {first_point, delta, static_cast<size_t>(number_of_entries)};
  }
  Interpolation_data interpolation_points_helper(
      double first_point, double desired_delta, double last_point) {
    auto number_of_entries = static_cast<size_t>(std::ceil(
                                 (last_point - first_point) / desired_delta))
                             + 1;
    auto delta
        = ((last_point - first_point)
           / (static_cast<double>(number_of_entries - 1)));
    return {first_point, delta, number_of_entries};
  }

  static std::vector<double>
  set_equidistant_interpolation_points(Interpolation_data data) {
    auto startpoint = data.first_point;
    auto delta = data.delta;
    auto number_of_entries = data.number_of_entries;
    std::vector<double> interpolation_points(number_of_entries);
    auto currenttime = startpoint;
    for (size_t index = 0; index != number_of_entries; ++index) {
      interpolation_points.push_back(currenttime);
      currenttime += delta;
    }
    return interpolation_points;
  }

  nlohmann::json Vector_interpolator::get_schema() {
    assert(false); // implement me!
    return nlohmann::json();
  }

  Vector_interpolator::Vector_interpolator(
      Interpolation_data data, Eigen::Index _inner_length) :
      interpolation_points(set_equidistant_interpolation_points(data)),
      inner_length(_inner_length) {
    if (inner_length < 0) {
      gthrow({"You can't have less than 0 controls per time step!"});
    }
    if (data.number_of_entries < 1) {
      gthrow({"You can't have less than 1 time step!"});
    }
    allcontrols = Eigen::VectorXd(
        _inner_length * static_cast<Eigen::Index>(data.number_of_entries));
  }

  Vector_interpolator::Vector_interpolator(
      std::vector<double> _interpolation_points, Eigen::Index _inner_length) :
      interpolation_points(_interpolation_points),
      inner_length(_inner_length),
      allcontrols(
          _inner_length
          * static_cast<Eigen::Index>(interpolation_points.size())) {}

  Vector_interpolator
  Vector_interpolator::construct_from_json(nlohmann::json const &json) {
    Aux::schema::validate_json(json, get_schema());

    std::vector<double> interpolation_points;

    std::string name;
    if (json.front().contains("x")) {
      name = "x";
    } else {
      name = "time";
    }
    for (auto const &[index, entry] : json.items()) {
      interpolation_points.push_back(entry["name"]);
    }

    auto inner_length
        = static_cast<Eigen::Index>(json.front()["values"].size());
    return Vector_interpolator(interpolation_points, inner_length);
  }

  void Vector_interpolator::set_controls(Eigen::Ref<Eigen::VectorXd> values) {
    if (values.size() != allcontrols.size()) {
      gthrow({"You are trying to assign the wrong number of controls."});
    }
    allcontrols = values;
  }

  Eigen::Index Vector_interpolator::get_number_of_controls() const {
    return allcontrols.size();
  }

  Eigen::Ref<Eigen::VectorXd const> const
  Vector_interpolator::operator()(double t) const {
    if (t >= interpolation_points.back()) {
      if (t > interpolation_points.back() + Aux::EPSILON) {
        std::ostringstream error_message;
        error_message << "Requested value " << t
                      << " is higher than the last valid value: "
                      << interpolation_points.back() << "\n";
        throw std::runtime_error(error_message.str());
      }

      return allcontrols.segment(
          static_cast<Eigen::Index>(interpolation_points.size() - 1)
              * inner_length,
          inner_length);
    }
    if (t >= interpolation_points.front()) {
      if (t > interpolation_points.front() + Aux::EPSILON) {
        std::ostringstream error_message;
        error_message << "Requested value " << t
                      << " is lower than the first valid value: "
                      << interpolation_points.front() << "\n";
        throw std::runtime_error(error_message.str());
      }

      return allcontrols.segment(0, inner_length);
    }
    auto it = std::lower_bound(
        interpolation_points.begin(), interpolation_points.end(), t);
    auto t_next = *it;
    auto t_prev = *std::prev(it);
    auto next_index = (it - interpolation_points.begin()) * inner_length;
    auto prev_index = next_index - inner_length;

    Eigen::VectorXd value_next = allcontrols.segment(next_index, inner_length);
    Eigen::VectorXd value_prev = allcontrols.segment(prev_index, inner_length);

    auto lambda = (t - t_prev) / (t_next - t_prev);

    return (1 - lambda) * value_prev + lambda * value_next;
  }

  Eigen::Ref<Eigen::VectorXd const> const
  Vector_interpolator::get_allcontrols() const {
    return allcontrols;
  }

  Eigen::Ref<Eigen::VectorXd>
  Vector_interpolator::mut_timestep(int current_timestep) {
    if (current_timestep < 0) {
      gthrow({"You request controls at negative time steps!"});
    }
    auto start_index = inner_length * current_timestep;
    auto after_index = start_index + inner_length;
    if (after_index > allcontrols.size()) {
      gthrow({"You request controls at a higher index than possible!"});
    }
    return allcontrols.segment(start_index, inner_length);
  }

} // namespace Aux
