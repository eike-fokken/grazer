#include "InterpolatingVector.hpp"

#include "Exception.hpp"
#include "Mathfunctions.hpp"
#include "make_schema.hpp"
#include "schema_validation.hpp"

#include <algorithm>

#include <stdexcept>
#include <string>

namespace Aux {

  Interpolation_data make_from_start_number_end(
      double first_point, double last_point, int number_of_entries) {
    if (number_of_entries <= 0) {
      gthrow(
          {"You can't have negative or zero number of entries in an "
           "InterpolatingVector.\n",
           "Supplied number of entries: ", std::to_string(number_of_entries)});
    }
    size_t number = static_cast<size_t>(number_of_entries);
    if (last_point <= first_point) {
      gthrow(
          {"You can't have negative or zero number of entries in an ",
           "InterpolatingVector.\n",
           "Supplied first point: ", std::to_string(first_point), "\n",
           "Supplied last point: ", std::to_string(last_point), "\n"});
    }
    auto delta = last_point - first_point / number_of_entries;
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
    return interpolation_points;
  }

  InterpolatingVector_Base::~InterpolatingVector_Base() {}

  InterpolatingVector_Base::InterpolatingVector_Base() :
      interpolation_points(), inner_length(0) {}
  InterpolatingVector_Base::InterpolatingVector_Base(
      Interpolation_data data, Eigen::Index _inner_length) :
      interpolation_points(set_equidistant_interpolation_points(data)),
      inner_length(_inner_length) {
    if (inner_length < 0) {
      gthrow({"You can't have less than 0 entries per interpolation point!"});
    }
    if (data.number_of_entries < 1) {
      gthrow({"You can't have less than 1 interpolation point!"});
    }
  }

  InterpolatingVector_Base::InterpolatingVector_Base(
      Eigen::Ref<Eigen::VectorXd const> const &_interpolation_points,
      Eigen::Index _inner_length) :
      interpolation_points(
          _interpolation_points.begin(), _interpolation_points.end()),
      inner_length(_inner_length) {
    if (not std::is_sorted(
            _interpolation_points.begin(), _interpolation_points.end())) {
      gthrow({"Interpolation points for InterpolatingVector were not sorted!"});
    }
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
      InterpolatingVector_Base &&other) {
    move_helper(std::move(other));
  }

  void InterpolatingVector_Base::move_helper(InterpolatingVector_Base &&other) {
    assert(*this != other);
    interpolation_points = std::move(other.interpolation_points);
    inner_length = other.inner_length;
    other.inner_length = 0;
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

  ////////////////////////////////////////////////////////////////
  // InterpolatingVector:
  ////////////////////////////////////////////////////////////////

  InterpolatingVector::InterpolatingVector() : InterpolatingVector_Base() {}
  InterpolatingVector::InterpolatingVector(
      Interpolation_data data, Eigen::Index _inner_length) :
      InterpolatingVector_Base(data, _inner_length),
      values(
          _inner_length * static_cast<Eigen::Index>(data.number_of_entries)) {}

  InterpolatingVector::InterpolatingVector(
      std::vector<double> _interpolation_points, Eigen::Index _inner_length) :
      InterpolatingVector_Base(_interpolation_points, _inner_length),
      values(
          _inner_length
          * static_cast<Eigen::Index>(_interpolation_points.size())) {}

  InterpolatingVector::InterpolatingVector(
      Eigen::Ref<Eigen::VectorXd const> const &_interpolation_points,
      Eigen::Index _inner_length) :
      InterpolatingVector_Base(_interpolation_points, _inner_length),
      values(
          _inner_length
          * static_cast<Eigen::Index>(_interpolation_points.size())) {}

  InterpolatingVector &
  InterpolatingVector::operator=(InterpolatingVector_Base const &other) {
    assignment_helper(other);
    this->values = other.get_allvalues();
    return *this;
  }

  InterpolatingVector &
  InterpolatingVector::operator=(InterpolatingVector const &other) {
    assignment_helper(other);
    this->values = other.get_allvalues();
    return *this;
  }

  InterpolatingVector &
  InterpolatingVector::operator=(InterpolatingVector &&other) {
    move_helper(std::move(other));
    values = std::move(other.values);
    return *this;
  }

  InterpolatingVector::InterpolatingVector(
      InterpolatingVector_Base const &other) :
      InterpolatingVector_Base(other), values(other.get_allvalues()) {}

  Eigen::Ref<Eigen::VectorXd> InterpolatingVector::allvalues() {
    return values;
  }

  Eigen::Ref<Eigen::VectorXd const> const
  InterpolatingVector::allvalues() const {
    return values;
  }

  InterpolatingVector InterpolatingVector::construct_and_interpolate_from(
      Eigen::VectorXd interpolation_points, Eigen::Index inner_length,
      InterpolatingVector_Base const &values) {
    InterpolatingVector result(interpolation_points, inner_length);
    result.interpolate_from(values);
    return result;
  }

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

  MappedInterpolatingVector::MappedInterpolatingVector(
      Interpolation_data data, Eigen::Index _inner_length, double *array,
      Eigen::Index number_of_elements) :
      InterpolatingVector_Base(data, _inner_length),
      mapped_values(array, number_of_elements) {
    if (get_inner_length() * size() != number_of_elements) {
      gthrow(
          {"Given number of elements of mapped storage differs from needed "
           "number of elements as given in other data in "
           "MappedInterpolatingVector!"});
    }
  }

  MappedInterpolatingVector::MappedInterpolatingVector(
      std::vector<double> _interpolation_points, Eigen::Index _inner_length,
      double *array, Eigen::Index number_of_elements) :
      InterpolatingVector_Base(_interpolation_points, _inner_length),
      mapped_values(array, number_of_elements) {
    if (get_inner_length() * size() != number_of_elements) {
      gthrow(
          {"Given number of elements of mapped storage differs from needed ",
           "number of elements as given in other data in ",
           "MappedInterpolatingVector!"});
    }
  }

  MappedInterpolatingVector::MappedInterpolatingVector(
      Eigen::Ref<Eigen::VectorXd const> const &_interpolation_points,
      Eigen::Index _inner_length, double *array,
      Eigen::Index number_of_elements) :
      InterpolatingVector_Base(_interpolation_points, _inner_length),
      mapped_values(array, number_of_elements) {
    if (get_inner_length() * size() != number_of_elements) {
      gthrow(
          {"Given number of elements of mapped storage differs from needed ",
           "number of elements as given in other data in ",
           "MappedInterpolatingVector!"});
    }
  }

  MappedInterpolatingVector &
  MappedInterpolatingVector::operator=(InterpolatingVector_Base const &other) {
    if (not have_same_structure(other, *this)) {
      gthrow(
          {"You are trying to assign an InterpolatingVector with a different "
           "structure to this InterpolatingVector.\nThis is not permitted "
           "through the InterpolatingVector_Base interface."});
    }
    allvalues() = other.get_allvalues();
    assignment_helper(other);
    return *this;
  }

  MappedInterpolatingVector &
  MappedInterpolatingVector::operator=(MappedInterpolatingVector const &other) {
    if (not have_same_structure(other, *this)) {
      gthrow(
          {"You are trying to assign an InterpolatingVector with a different "
           "structure to this MappedInterpolatingVector.\nThis is not "
           "permitted."});
    }
    allvalues() = other.mapped_values;
    assignment_helper(other);
    return *this;
  }

  Eigen::Ref<Eigen::VectorXd> MappedInterpolatingVector::allvalues() {
    return mapped_values;
  }
  Eigen::Ref<Eigen::VectorXd const> const
  MappedInterpolatingVector::allvalues() const {
    return mapped_values;
  }

  ConstMappedInterpolatingVector::ConstMappedInterpolatingVector(
      Interpolation_data data, Eigen::Index _inner_length, double const *array,
      Eigen::Index number_of_elements) :
      InterpolatingVector_Base(data, _inner_length),
      mapped_values(array, number_of_elements) {
    if (get_inner_length() * size() != number_of_elements) {
      gthrow(
          {"Given number of elements of mapped storage differs from needed "
           "number of elements as given in other data in "
           "MappedInterpolatingVector!"});
    }
  }

  ConstMappedInterpolatingVector::ConstMappedInterpolatingVector(
      std::vector<double> _interpolation_points, Eigen::Index _inner_length,
      double const *array, Eigen::Index number_of_elements) :
      InterpolatingVector_Base(_interpolation_points, _inner_length),
      mapped_values(array, number_of_elements) {
    if (get_inner_length() * size() != number_of_elements) {
      gthrow(
          {"Given number of elements of mapped storage differs from needed ",
           "number of elements as given in other data in ",
           "MappedInterpolatingVector!"});
    }
  }

  ConstMappedInterpolatingVector::ConstMappedInterpolatingVector(
      Eigen::Ref<Eigen::VectorXd const> const &_interpolation_points,
      Eigen::Index _inner_length, double const *array,
      Eigen::Index number_of_elements) :
      InterpolatingVector_Base(_interpolation_points, _inner_length),
      mapped_values(array, number_of_elements) {
    if (get_inner_length() * size() != number_of_elements) {
      gthrow(
          {"Given number of elements of mapped storage differs from needed ",
           "number of elements as given in other data in ",
           "MappedInterpolatingVector!"});
    }
  }

  void ConstMappedInterpolatingVector::reset_values(
      double const *array, Eigen::Index number_of_elements) {
    if (number_of_elements != this->get_total_number_of_values()) {
      gthrow(
          {"You can only reset ConstMappedInterpolatingVector to an array of "
           "the same size as before."});
    }
    new (&mapped_values)
        Eigen::Map<Eigen::VectorXd const>(array, number_of_elements);
  }

  Eigen::Ref<Eigen::VectorXd> ConstMappedInterpolatingVector::allvalues() {
    gthrow(
        {"You are trying to get a mutable reference to mapped values in ",
         "ConstMappedInterpolatingVector.\n", "This is forbidden.",
         "The fact that you see this error message means, that somewhere in ",
         "your code a non-const object of type ConstMappedInterpolatingVector ",
         "is constructed. This is allowed but you may not call any non-const ",
         "method in InterpolatingVector_Base on such object!\n",
         "As of now, forbidden (non-const methods) are:\n",
         "mut_timestep(...)\n", "push_to_index(...)\n",
         "set_values_in_bulk(...)"});
  }
  Eigen::Ref<Eigen::VectorXd const> const
  ConstMappedInterpolatingVector::allvalues() const {
    return mapped_values;
  }

} // namespace Aux
