/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
#include "InterpolatingVector_Base.hpp"

#include "Exception.hpp"
#include "Mathfunctions.hpp"
#include "Misc.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

#include <algorithm>

#include <stdexcept>
#include <string>
#include <valarray>

namespace Aux {

  Interpolation_data make_from_start_number_end(
      double first_point, double last_point, int number_of_entries) {
    if (number_of_entries <= 0) {
      gthrow(
          {"You can't have negative or zero number of entries in an "
           "InterpolatingVector.\n",
           "Supplied number of entries: ", std::to_string(number_of_entries)});
    }
    if (last_point <= first_point) {
      gthrow(
          {"You can't have negative or zero number of entries in an ",
           "InterpolatingVector.\n",
           "Supplied first point: ", std::to_string(first_point), "\n",
           "Supplied last point: ", std::to_string(last_point), "\n"});
    }
    size_t number = static_cast<size_t>(number_of_entries);

    auto delta = (last_point - first_point) / (number_of_entries - 1);
    assert(
        last_point
        > first_point + (number_of_entries - 1) * delta - Aux::EPSILON);
    assert(
        last_point
        < first_point + (number_of_entries - 1) * delta + Aux::EPSILON);
    return {first_point, delta, number};
  }

  Interpolation_data make_from_start_delta_number(
      double first_point, double delta, int number_of_entries) {
    if (number_of_entries <= 0) {
      gthrow(
          {"You can't have negative or zero number of entries in an "
           "InterpolatingVector.\n",
           "Supplied number of entries: ", std::to_string(number_of_entries)});
    }
    if (delta <= 0) {
      gthrow(
          {"You can't have negative or zero step size in an "
           "InterpolatingVector.\n",
           "Supplied stepsize: ", std::to_string(delta)});
    }
    return {first_point, delta, static_cast<size_t>(number_of_entries)};
  }
  Interpolation_data make_from_start_delta_end(
      double first_point, double desired_delta, double last_point) {
    if (last_point <= first_point) {
      gthrow(
          {"You can't have negative or zero number of entries in an ",
           "InterpolatingVector.\n",
           "Supplied first point: ", std::to_string(first_point), "\n",
           "Supplied last point: ", std::to_string(last_point), "\n"});
    }
    if (desired_delta <= 0) {
      gthrow(
          {"You can't have negative or zero step size in an "
           "InterpolatingVector.\n",
           "Supplied desired stepsize: ", std::to_string(desired_delta)});
    }

    auto number_of_entries_double
        = (std::ceil((last_point - first_point) / desired_delta)) + 1;

    auto number_of_entries = static_cast<size_t>(number_of_entries_double);

    auto delta
        = ((last_point - first_point)
           / (static_cast<double>(number_of_entries - 1)));
    assert(
        last_point > first_point
                         + static_cast<double>(number_of_entries - 1) * delta
                         - Aux::EPSILON);
    assert(
        last_point < first_point
                         + static_cast<double>(number_of_entries - 1) * delta
                         + Aux::EPSILON);

    return {first_point, delta, number_of_entries};
  }

  static std::vector<double>
  set_equidistant_interpolation_points(Interpolation_data data) {
    auto startpoint = data.first_point;
    auto delta = data.delta;
    auto number_of_entries = data.number_of_entries;
    std::vector<double> interpolation_points;
    auto currentpoint = startpoint;
    for (size_t index = 0; index != number_of_entries; ++index) {
      interpolation_points.push_back(currentpoint);
      currentpoint += delta;
    }
    assert(interpolation_points.front() == startpoint);
    // check for double equality. Should be an aux function...
    assert(
        (std::abs(
            interpolation_points.back()
            - (startpoint
               + static_cast<double>(number_of_entries - 1) * delta)))
        < Aux::EPSILON);
    return interpolation_points;
  }

  InterpolatingVector_Base::~InterpolatingVector_Base() {}

  InterpolatingVector_Base::InterpolatingVector_Base() :
      interpolation_points(), inner_length(0) {}
  InterpolatingVector_Base::InterpolatingVector_Base(
      Interpolation_data data, Eigen::Index _inner_length) :
      interpolation_points(set_equidistant_interpolation_points(data)),
      inner_length(_inner_length) {
    // assert(
    //     (((inner_length > 0) and (interpolation_points.size()>0))
    //      or ((inner_length == 0) and interpolation_points.empty()))
    //     and "Either inner length and number of interpolation points must be
    //     zero or both must be greater in an InterpolatingVector.");
    assert(
        std::is_sorted(interpolation_points.begin(), interpolation_points.end())
        and "Interpolation points for InterpolatingVector were not sorted!");
  }

  InterpolatingVector_Base::InterpolatingVector_Base(
      Eigen::Ref<Eigen::VectorXd const> const &_interpolation_points,
      Eigen::Index _inner_length) :
      interpolation_points(
          _interpolation_points.begin(), _interpolation_points.end()),
      inner_length(_inner_length) {
    // assert(
    //     (((inner_length > 0) and (interpolation_points.size()>0))
    //      or ((inner_length == 0) and interpolation_points.empty()))
    //     and "Either inner length and number of interpolation points must be
    //     zero or both must be greater in an InterpolatingVector.");
    assert(
        std::is_sorted(interpolation_points.begin(), interpolation_points.end())
        and "Interpolation points for InterpolatingVector were not sorted!");
  }

  InterpolatingVector_Base::InterpolatingVector_Base(
      std::vector<double> _interpolation_points, Eigen::Index _inner_length) :
      interpolation_points(std::move(_interpolation_points)),
      inner_length(_inner_length) {
    if (not std::is_sorted(
            _interpolation_points.begin(), _interpolation_points.end())) {
      gthrow({"Interpolation points for InterpolatingVector were not sorted!"});
    }
  }

  void InterpolatingVector_Base::assignment_helper(
      InterpolatingVector_Base const &other) {
    interpolation_points = other.interpolation_points;
    inner_length = other.inner_length;
  }

  InterpolatingVector_Base::InterpolatingVector_Base(
      InterpolatingVector_Base &&other) noexcept :
      interpolation_points(std::move(other.interpolation_points)),
      inner_length(other.inner_length) {
    other.inner_length = 0;
  }

  InterpolatingVector_Base &InterpolatingVector_Base::operator=(
      InterpolatingVector_Base &&other) noexcept {
    assert(this != &other);
    interpolation_points = std::move(other.interpolation_points);
    inner_length = other.inner_length;
    other.inner_length = 0;
    return *this;
  }

  InterpolatingVector_Base &
  InterpolatingVector_Base::operator=(InterpolatingVector_Base const &other) {
    if (not have_same_structure(other, *this)) {
      gthrow(
          {"You are trying to assign an InterpolatingVector with a different "
           "structure to this InterpolatingVector.\nThis is not permitted "
           "through the InterpolatingVector_Base interface."});
    }
    assignment_helper(other);
    allvalues() = other.get_allvalues();
    return *this;
  }

  void InterpolatingVector_Base::set_values_in_bulk(
      Eigen::Ref<Eigen::VectorXd const> const &values) {
    if (values.size() != allvalues().size()) {
      gthrow({"You are trying to assign the wrong number of values."});
    }
    allvalues() = values;
  }

  void InterpolatingVector_Base::setZero() { allvalues().setZero(); }

  void InterpolatingVector_Base::interpolate_from(
      InterpolatingVector_Base const &values) {

    assert(get_inner_length() == values.get_inner_length());
    assert(
        values.interpolation_point_at_index(0)
        <= interpolation_point_at_index(0));
    assert(
        values.interpolation_point_at_index(values.size() - 1)
        >= interpolation_point_at_index(size() - 1));

    for (Eigen::Index index = 0; index != size(); ++index) {
      mut_timestep(index) = values(interpolation_point_at_index(index));
    }
  }

  Eigen::Index InterpolatingVector_Base::get_total_number_of_values() const {
    return get_allvalues().size();
  }

  Eigen::Index InterpolatingVector_Base::get_inner_length() const {
    return inner_length;
  }

  Eigen::Map<Eigen::VectorXd const>
  InterpolatingVector_Base::get_interpolation_points() const {
    return Eigen::Map<Eigen::VectorXd const>(
        interpolation_points.data(), size());
  }

  std::vector<double> const &
  InterpolatingVector_Base::get_interpolation_points_vector() const {
    return interpolation_points;
  }

  Eigen::VectorXd InterpolatingVector_Base::operator()(double t) const {
    if (t >= interpolation_points.back()) {
      if (t > interpolation_points.back() + Aux::EPSILON) {
        std::ostringstream error_message;
        error_message << "Requested value " << t
                      << " is higher than the last valid value: "
                      << interpolation_points.back() << "\n";
        throw std::runtime_error(error_message.str());
      }

      return get_allvalues().segment(
          static_cast<Eigen::Index>(interpolation_points.size() - 1)
              * inner_length,
          inner_length);
    }
    if (t <= interpolation_points.front()) {
      if (t < interpolation_points.front() - Aux::EPSILON) {
        std::ostringstream error_message;
        error_message << "Requested value " << t
                      << " is lower than the first valid value: "
                      << interpolation_points.front() << "\n";
        throw std::runtime_error(error_message.str());
      }

      return get_allvalues().segment(0, inner_length);
    }
    auto it = std::lower_bound(
        interpolation_points.begin(), interpolation_points.end(), t);
    auto t_next = *it;
    auto t_prev = *std::prev(it);
    auto next_index = static_cast<Eigen::Index>(
        (it - interpolation_points.begin()) * inner_length);
    auto prev_index = next_index - inner_length;

    Eigen::VectorXd value_next
        = get_allvalues().segment(next_index, inner_length);
    Eigen::VectorXd value_prev
        = get_allvalues().segment(prev_index, inner_length);

    auto lambda = (t - t_prev) / (t_next - t_prev);

    return (1 - lambda) * value_prev + lambda * value_next;
  }

  Eigen::Ref<Eigen::VectorXd const> const
  InterpolatingVector_Base::get_allvalues() const {
    return allvalues();
  }

  Eigen::Ref<Eigen::VectorXd>
  InterpolatingVector_Base::mut_timestep(Eigen::Index current_index) {
    if (current_index < 0) {
      gthrow({"You try to set this InterpolatingVector at a negative index!"});
    }
    auto start_index = inner_length * current_index;
    auto after_index = start_index + inner_length;
    if (after_index > get_allvalues().size()) {
      gthrow(
          {"You try to set this InterpolatingVector at a higher index than "
           "possible!"});
    }
    return allvalues().segment(start_index, inner_length);
  }

  void InterpolatingVector_Base::push_to_index(
      Eigen::Index index, double timepoint, Eigen::VectorXd vector) {
    mut_timestep(index) = vector;
    interpolation_points[static_cast<size_t>(index)] = timepoint;
  }

  Eigen::Ref<Eigen::VectorXd const>
  InterpolatingVector_Base::vector_at_index(Eigen::Index current_index) const {
    if (current_index < 0) {
      gthrow(
          {"You try to access this InterpolatingVector at a negative index!"});
    }
    auto start_index = inner_length * current_index;
    auto after_index = start_index + inner_length;
    if (after_index > get_allvalues().size()) {
      gthrow(
          {"You try to set this InterpolatingVector at a higher index than "
           "possible!"});
    }
    return get_allvalues().segment(start_index, inner_length);
  }

  double InterpolatingVector_Base::interpolation_point_at_index(
      Eigen::Index index) const {
    return interpolation_points[static_cast<size_t>(index)];
  }

  Eigen::Index InterpolatingVector_Base::size() const {
    return static_cast<Eigen::Index>(interpolation_points.size());
  }

  bool operator==(
      InterpolatingVector_Base const &lhs,
      InterpolatingVector_Base const &rhs) {
    if (not have_same_structure(lhs, rhs)) {
      return false;
    }
    return lhs.get_allvalues() == rhs.get_allvalues();
  }

  bool operator!=(
      InterpolatingVector_Base const &lhs,
      InterpolatingVector_Base const &rhs) {
    return !(lhs == rhs);
  }

  bool have_same_structure(
      InterpolatingVector_Base const &vec1,
      InterpolatingVector_Base const &vec2) {
    if (vec1.get_inner_length() != vec2.get_inner_length()) {
      return false;
    }
    if (vec1.get_interpolation_points().size()
        != vec2.get_interpolation_points().size()) {
      return false;
    }
    if (vec1.get_interpolation_points() != vec2.get_interpolation_points()) {
      return false;
    }
    return true;
  }

} // namespace Aux
