#pragma once

#include "InterpolatingVector_Base.hpp"

namespace Aux {

  class MappedInterpolatingVector : public InterpolatingVector_Base {
  public:
    MappedInterpolatingVector(
        Interpolation_data data, Eigen::Index _inner_length, double *array,
        Eigen::Index number_of_elements);
    MappedInterpolatingVector(
        std::vector<double> interpolation_points, Eigen::Index inner_length,
        double *array, Eigen::Index number_of_elements);
    MappedInterpolatingVector(
        Eigen::Ref<Eigen::VectorXd const> const &interpolation_points,
        Eigen::Index inner_length, double *array,
        Eigen::Index number_of_elements);

    // We cannot have a copy constructor because a MappedInterpolatingVector
    // needs an underlying storage.
    MappedInterpolatingVector(MappedInterpolatingVector const &other) = delete;

    /** @brief Copy assignment operator.
     *
     * Throws if the underlying storages of
     * the left-hand operand does not have the same total size as the right-hand
     * operand.
     */
    MappedInterpolatingVector &operator=(InterpolatingVector_Base const &other);
    /** @brief Copy assignment operator.
     *
     * Throws if the underlying storages of
     * the left-hand operand does not have the same total size as the right-hand
     * operand.
     */
    MappedInterpolatingVector &
    operator=(MappedInterpolatingVector const &other);

  private:
    Eigen::Ref<Eigen::VectorXd> allvalues() final;
    Eigen::Ref<Eigen::VectorXd const> const allvalues() const final;

    Eigen::Map<Eigen::VectorXd> mapped_values;
  };

  class ConstMappedInterpolatingVector : public InterpolatingVector_Base {
  public:
    ConstMappedInterpolatingVector(
        Interpolation_data data, Eigen::Index _inner_length,
        double const *array, Eigen::Index number_of_elements);
    ConstMappedInterpolatingVector(
        std::vector<double> interpolation_points, Eigen::Index inner_length,
        double const *array, Eigen::Index number_of_elements);
    ConstMappedInterpolatingVector(
        Eigen::Ref<Eigen::VectorXd const> const &interpolation_points,
        Eigen::Index inner_length, double const *array,
        Eigen::Index number_of_elements);

    void reset_values(double const *array, Eigen::Index number_of_elements);

  private:
    Eigen::Ref<Eigen::VectorXd> allvalues() final;
    Eigen::Ref<Eigen::VectorXd const> const allvalues() const final;

    Eigen::Map<Eigen::VectorXd const> mapped_values;
  };

} // namespace Aux
