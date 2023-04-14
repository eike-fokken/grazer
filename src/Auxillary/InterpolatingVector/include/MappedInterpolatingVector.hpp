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

    /** \brief provide a view into another InterpolatingVector.
     *
     * @param supervector The vector of which to create the view.
     * @param start_index The first index to be part of the view.
     * @param after_index The end index of the view, first index not to be part
     * of the view.
     */
    MappedInterpolatingVector(
        InterpolatingVector_Base &supervector, Eigen::Index start_index,
        Eigen::Index after_index);

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

    /** \brief give derived classes access to the pointer to the start of the
     * data
     *
     * @returns pointer to the start of all values.
     */
    double *get_value_pointer() final;

    /** \brief give derived classes access to the pointer to the start of the
     * data
     * @returns pointer to the start of all values.
     */
    double const *get_value_pointer() const final;

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

    ConstMappedInterpolatingVector(
        InterpolatingVector_Base const &supervector, Eigen::Index start_index,
        Eigen::Index after_index);

    void reset_values(double const *array, Eigen::Index number_of_elements);

    /** \brief give derived classes access to the pointer to the start of the
     * data
     *
     * @returns pointer to the start of all values.
     */
    double *get_value_pointer() final;

    /** \brief give derived classes access to the pointer to the start of the
     * data
     * @returns pointer to the start of all values.
     */
    double const *get_value_pointer() const final;

  private:
    Eigen::Ref<Eigen::VectorXd> allvalues() final;
    Eigen::Ref<Eigen::VectorXd const> const allvalues() const final;

    Eigen::Map<Eigen::VectorXd const> mapped_values;
  };

} // namespace Aux
