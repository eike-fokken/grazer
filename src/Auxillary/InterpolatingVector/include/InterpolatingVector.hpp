#pragma once
#include "Timedata.hpp"
#include <Eigen/Sparse>
#include <cstddef>
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

  class InterpolatingVector {
  public:
    InterpolatingVector();
    InterpolatingVector(InterpolatingVector const &other) = default;
    InterpolatingVector(Interpolation_data data, Eigen::Index _inner_length);
    InterpolatingVector(
        std::vector<double> interpolation_points, Eigen::Index inner_length);

    static InterpolatingVector construct_from_json(
        nlohmann::json const &json, nlohmann::json const &schema);

    void set_values_in_bulk(Eigen::Ref<Eigen::VectorXd> values);

    Eigen::Index get_total_number_of_values() const;
    std::vector<double> const &get_interpolation_points() const;

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
    std::vector<double> interpolation_points;
    Eigen::Index inner_length;
    Eigen::VectorXd allvalues;
  };
  bool
  operator==(InterpolatingVector const &lhs, InterpolatingVector const &rhs);
  bool
  operator!=(InterpolatingVector const &lhs, InterpolatingVector const &rhs);

  /** @brief returns true if and only if both arguments have the same inner
   * length and the same interpolation points.
   */
  bool have_same_structure(
      InterpolatingVector const &vec1, InterpolatingVector const &vec2);

} // namespace Aux
