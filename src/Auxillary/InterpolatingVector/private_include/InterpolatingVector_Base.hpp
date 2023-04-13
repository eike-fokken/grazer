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
#pragma once
#include "Timedata.hpp"
#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
#include <vector>

namespace Aux {

  struct Interpolation_data {
    double first_point;
    double delta;
    size_t number_of_entries;
  };
  Interpolation_data make_from_start_delta_number(
      double first_point, double delta, int number_of_entries);

  Interpolation_data make_from_start_delta_end(
      double first_point, double desired_delta, double last_point);

  Interpolation_data make_from_start_number_end(
      double first_point, double last_point, int number_of_entries);

  class InterpolatingVector_Base {

  public:
    virtual ~InterpolatingVector_Base();
    InterpolatingVector_Base();
    InterpolatingVector_Base(
        Interpolation_data data, Eigen::Index _inner_length);
    InterpolatingVector_Base(
        std::vector<double> interpolation_points, Eigen::Index inner_length);
    InterpolatingVector_Base(
        Eigen::Ref<Eigen::VectorXd const> const &interpolation_points,
        Eigen::Index inner_length);

    InterpolatingVector_Base &operator=(InterpolatingVector_Base const &other);

  protected:
    /// @brief helper function for implementing the assignment in derived
    /// classes
    void assignment_helper(InterpolatingVector_Base const &other);
    InterpolatingVector_Base &
    operator=(InterpolatingVector_Base &&other) noexcept;

    /// @brief copy constructor, only callable from derived classes for copying
    /// private data.
    InterpolatingVector_Base(InterpolatingVector_Base const &other) = default;
    // move constructor, also only callable from derived classes:
    InterpolatingVector_Base(InterpolatingVector_Base &&other) noexcept;

  public:
    /** \brief give derived classes access to the pointer to the start of the
     * data
     *
     * @returns pointer to the start of all values.
     */
    virtual double *get_value_pointer() = 0;

    /** \brief give derived classes access to the pointer to the start of the
     * data
     * @returns pointer to the start of all values.
     */
    virtual double const *get_value_pointer() const = 0;

    void set_values_in_bulk(Eigen::Ref<Eigen::VectorXd const> const &values);
    void setZero();

    void interpolate_from(InterpolatingVector_Base const &);

    Eigen::Index get_total_number_of_values() const;
    Eigen::Map<Eigen::VectorXd const> get_interpolation_points() const;
    std::vector<double> const &get_interpolation_points_vector() const;

    Eigen::Index get_inner_length() const;

    Eigen::VectorXd operator()(double time) const;

    Eigen::Ref<Eigen::VectorXd const> const get_allvalues() const;

    Eigen::Ref<Eigen::VectorXd> mut_timestep(Eigen::Index index);

    void
    push_to_index(Eigen::Index index, double timepoint, Eigen::VectorXd vector);

    Eigen::Ref<Eigen::VectorXd const> vector_at_index(Eigen::Index index) const;
    double interpolation_point_at_index(Eigen::Index index) const;

    /** \brief Returns the number of timepoints or equivalently the number of
     * inner vectors.
     */
    Eigen::Index size() const;

  private:
    virtual Eigen::Ref<Eigen::VectorXd> allvalues() = 0;
    virtual Eigen::Ref<Eigen::VectorXd const> const allvalues() const = 0;

    std::vector<double> interpolation_points;
    Eigen::Index inner_length;
  };

  bool operator==(
      InterpolatingVector_Base const &lhs, InterpolatingVector_Base const &rhs);
  bool operator!=(
      InterpolatingVector_Base const &lhs, InterpolatingVector_Base const &rhs);

  /** @brief returns true if and only if both arguments have the same inner
   * length and the same interpolation points.
   */
  bool have_same_structure(
      InterpolatingVector_Base const &vec1,
      InterpolatingVector_Base const &vec2);

} // namespace Aux
