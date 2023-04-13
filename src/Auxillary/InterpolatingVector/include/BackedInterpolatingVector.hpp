#pragma once

#include "InterpolatingVector_Base.hpp"

namespace Aux {
  class InterpolatingVector : public InterpolatingVector_Base {
  public:
    static InterpolatingVector construct_and_interpolate_from(
        Eigen::VectorXd _interpolation_points, Eigen::Index inner_length,
        InterpolatingVector_Base const &values);

    static InterpolatingVector construct_from_json(
        nlohmann::json const &json, nlohmann::json const &schema);

    InterpolatingVector();
    InterpolatingVector(Interpolation_data data, Eigen::Index inner_length);
    InterpolatingVector(
        std::vector<double> _interpolation_points, Eigen::Index inner_length);
    InterpolatingVector(
        Eigen::Ref<Eigen::VectorXd const> const &_interpolation_points,
        Eigen::Index inner_length);

    // assignment:
    InterpolatingVector &operator=(InterpolatingVector_Base const &other);
    InterpolatingVector &operator=(InterpolatingVector const &other);
    InterpolatingVector &operator=(InterpolatingVector &&other) = default;
    // copy constructor:
    InterpolatingVector(InterpolatingVector_Base const &other);
    InterpolatingVector(InterpolatingVector const &other) = default;
    // move constructor:
    InterpolatingVector(InterpolatingVector &&other) noexcept;

  private:
    Eigen::Ref<Eigen::VectorXd> allvalues() final;
    Eigen::Ref<Eigen::VectorXd const> const allvalues() const final;

    Eigen::VectorXd values;
  };

} // namespace Aux
