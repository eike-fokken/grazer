#include "InterpolatingVector.hpp"

#include "Exception.hpp"
#include "Mathfunctions.hpp"
#include "Timedata.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

#include <algorithm>
#include <cstddef>
#include <exception>
#include <iostream>
#include <limits>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>

namespace Aux {

  Interpolation_data interpolation_points_helper(
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
  Interpolation_data interpolation_points_helper(
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

    size_t number_of_entries = 0;
    if (static_cast<double>(static_cast<size_t>(number_of_entries_double))
        != number_of_entries_double) {
      gthrow(
          {"The number of interpolation points is too big and cannot fit into "
           "an "
           "Eigen::VectorXd! Its size is greater than the maximimum of "
           "Eigen::Index"});
    } else {
      number_of_entries = static_cast<size_t>(number_of_entries_double);
      if (number_of_entries > std::numeric_limits<Eigen::Index>::max()) {
        gthrow(
            {"The number of interpolation points is too big and cannot fit "
             "into "
             "an "
             "Eigen::VectorXd! Its size is greater than the maximimum of "
             "Eigen::Index"});
      }
    }

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
    std::vector<double> interpolation_points;
    auto currentpoint = startpoint;
    for (size_t index = 0; index != number_of_entries; ++index) {
      interpolation_points.push_back(currentpoint);
      currentpoint += delta;
    }
    if (interpolation_points.size()
        > std::numeric_limits<Eigen::Index>::max()) {
      gthrow(
          {"The interpolation point vector is too big and cannot fit into an "
           "Eigen::VectorXd! Its size is greater than the maximimum of "
           "Eigen::Index"});
    }
    return interpolation_points;
  }

  nlohmann::json
  InterpolatingVector::get_schema(size_t number_of_values_per_point) {
    nlohmann::json element_schema;
    element_schema["type"] = "object";
    // This doesn't prohibit to have interchanging "x" and "time" properties,
    // but hopefully noone will try that...
    element_schema["additionalProperties"] = false;
    Aux::schema::add_property(element_schema, "x", Aux::schema::type::number());
    Aux::schema::add_property(
        element_schema, "time", Aux::schema::type::number());
    Aux::schema::add_required(
        element_schema, "values",
        Aux::schema::make_list_schema_of(
            Aux::schema::type::number(),
            {{"description", "Initial Values at an interpolation point"},
             {"minItems", number_of_values_per_point},
             {"maxItems", number_of_values_per_point}}));

    auto schema = Aux::schema::make_list_schema_of(element_schema);
    return schema;
  }

  InterpolatingVector::InterpolatingVector(
      Interpolation_data data, Eigen::Index _inner_length) :
      interpolation_points(set_equidistant_interpolation_points(data)),
      inner_length(_inner_length) {
    if (inner_length < 0) {
      gthrow({"You can't have less than 0 entries per interpolation point!"});
    }
    if (data.number_of_entries < 1) {
      gthrow({"You can't have less than 1 interpolation point!"});
    }
    allvalues = Eigen::VectorXd(
        _inner_length * static_cast<Eigen::Index>(data.number_of_entries));
    if (interpolation_points.size()
        > std::numeric_limits<Eigen::Index>::max()) {
      gthrow(
          {"The vector index of inner_length too big to fit into "
           "Eigen::Index."});
    }
  }

  InterpolatingVector::InterpolatingVector(
      std::vector<double> _interpolation_points, Eigen::Index _inner_length) :
      interpolation_points(_interpolation_points),
      inner_length(_inner_length),
      allvalues(
          _inner_length
          * static_cast<Eigen::Index>(interpolation_points.size())) {}

  InterpolatingVector InterpolatingVector::construct_from_json(
      nlohmann::json const &json, nlohmann::json const &schema) {
    Aux::schema::validate_json(json, schema);

    std::vector<double> interpolation_points;

    std::string name;
    if (json["data"].front().contains("x")) {
      name = "x";
    } else {
      name = "time";
    }
    for (auto const &[index, entry] : json["data"].items()) {
      interpolation_points.push_back(entry[name]);
    }

    auto inner_length
        = static_cast<Eigen::Index>(json["data"].front()["values"].size());
    InterpolatingVector interpolatingVector(interpolation_points, inner_length);

    auto size = interpolation_points.size();
    for (size_t i = 0; i != size; ++i) {
      Eigen::VectorXd a(json["data"][i]["values"].size());
      for (size_t index = 0; index != json["data"][i]["values"].size();
           ++index) {
        a[static_cast<Eigen::Index>(index)]
            = json["data"][i]["values"][index].get<double>();
      }
      interpolatingVector.mut_timestep(static_cast<Eigen::Index>(i)) = a;
    }
    return interpolatingVector;

    gthrow(
        {"Couldn't determine the right number of entries in ",
         "InterpolatingVectors passed json in component ",
         json["id"].get<std::string>(), ".\n",
         "The json was: ", json.dump(1, '\t')});
  }

  void
  InterpolatingVector::set_values_in_bulk(Eigen::Ref<Eigen::VectorXd> values) {
    if (values.size() != allvalues.size()) {
      gthrow({"You are trying to assign the wrong number of values."});
    }
    allvalues = values;
  }

  Eigen::Index InterpolatingVector::get_total_number_of_values() const {
    return allvalues.size();
  }

  Eigen::Index InterpolatingVector::get_inner_length() const {
    return inner_length;
  }

  std::vector<double> const &
  InterpolatingVector::get_interpolation_points() const {
    return interpolation_points;
  }

  Eigen::VectorXd InterpolatingVector::operator()(double t) const {
    if (t >= interpolation_points.back()) {
      if (t > interpolation_points.back() + Aux::EPSILON) {
        std::ostringstream error_message;
        error_message << "Requested value " << t
                      << " is higher than the last valid value: "
                      << interpolation_points.back() << "\n";
        throw std::runtime_error(error_message.str());
      }

      return allvalues.segment(
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

      return allvalues.segment(0, inner_length);
    }
    auto it = std::lower_bound(
        interpolation_points.begin(), interpolation_points.end(), t);
    auto t_next = *it;
    auto t_prev = *std::prev(it);
    auto next_index = static_cast<Eigen::Index>(
        (it - interpolation_points.begin()) * inner_length);
    auto prev_index = next_index - inner_length;

    Eigen::VectorXd value_next = allvalues.segment(next_index, inner_length);
    Eigen::VectorXd value_prev = allvalues.segment(prev_index, inner_length);

    auto lambda = (t - t_prev) / (t_next - t_prev);

    return (1 - lambda) * value_prev + lambda * value_next;
  }

  Eigen::Ref<Eigen::VectorXd const> const
  InterpolatingVector::get_allvalues() const {
    return allvalues;
  }

  Eigen::Ref<Eigen::VectorXd>
  InterpolatingVector::mut_timestep(Eigen::Index current_index) {
    if (current_index < 0) {
      gthrow({"You try to set this InterpolatingVector at a negative index!"});
    }
    auto start_index = inner_length * current_index;
    auto after_index = start_index + inner_length;
    if (after_index > allvalues.size()) {
      gthrow(
          {"You try to set this InterpolatingVector at a higher index than "
           "possible!"});
    }
    return allvalues.segment(start_index, inner_length);
  }

  Eigen::Ref<Eigen::VectorXd const>
  InterpolatingVector::const_timestep(Eigen::Index current_index) const {
    if (current_index < 0) {
      gthrow({"You try to set this InterpolatingVector at a negative index!"});
    }
    auto start_index = inner_length * current_index;
    auto after_index = start_index + inner_length;
    if (after_index > allvalues.size()) {
      gthrow(
          {"You try to set this InterpolatingVector at a higher index than "
           "possible!"});
    }
    return allvalues.segment(start_index, inner_length);
  }

} // namespace Aux
