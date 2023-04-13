#include "BackedInterpolatingVector.hpp"
#include "schema_validation.hpp"

namespace Aux {
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
    this->values = other.values;
    return *this;
  }

  double *InterpolatingVector::get_value_pointer() { return values.data(); }

  double const *InterpolatingVector::get_value_pointer() const {
    return values.data();
  }

  InterpolatingVector::InterpolatingVector(
      InterpolatingVector_Base const &other) :
      InterpolatingVector_Base(other), values(other.get_allvalues()) {}

  Eigen::Ref<Eigen::VectorXd> InterpolatingVector::allvalues() {
    return values;
  }

  InterpolatingVector::InterpolatingVector(
      InterpolatingVector &&other) noexcept = default;

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
  }

} // namespace Aux
