#include "MappedInterpolatingVector.hpp"
#include "Exception.hpp"
#include <cstddef>

namespace Aux {

  static std::vector<double> get_view_interpolation_points(
      InterpolatingVector_Base const &supervector, std::ptrdiff_t start_index,
      std::ptrdiff_t after_index) {
    auto start_iterator
        = supervector.get_interpolation_points_vector().begin() + start_index;
    auto end_iterator
        = supervector.get_interpolation_points_vector().begin() + after_index;

    return std::vector<double>(start_iterator, end_iterator);
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

  MappedInterpolatingVector::MappedInterpolatingVector(
      InterpolatingVector_Base &supervector, Eigen::Index start_index,
      Eigen::Index after_index) :
      InterpolatingVector_Base(
          get_view_interpolation_points(supervector, start_index, after_index),
          supervector.get_inner_length()),
      mapped_values(
          supervector.get_value_pointer()
              + start_index * supervector.get_inner_length(),
          (after_index - start_index) * supervector.get_inner_length()) {}

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

  double *MappedInterpolatingVector::get_value_pointer() {
    return mapped_values.data();
  }

  double const *MappedInterpolatingVector::get_value_pointer() const {
    return mapped_values.data();
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

  ConstMappedInterpolatingVector::ConstMappedInterpolatingVector(
      InterpolatingVector_Base const &supervector, Eigen::Index start_index,
      Eigen::Index after_index) :
      InterpolatingVector_Base(
          get_view_interpolation_points(supervector, start_index, after_index),
          supervector.get_inner_length()),
      mapped_values(
          supervector.get_value_pointer()
              + start_index * supervector.get_inner_length(),
          (after_index - start_index) * supervector.get_inner_length()) {}

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

  double *ConstMappedInterpolatingVector::get_value_pointer() {
    gthrow(
        {"You cannot call this function on a non-const instance of a "
         "ConstMappedInterpolatingVector! This is a bug!"});
  }

  double const *ConstMappedInterpolatingVector::get_value_pointer() const {
    return mapped_values.data();
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
