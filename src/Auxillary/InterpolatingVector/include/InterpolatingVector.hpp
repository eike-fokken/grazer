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

  Interpolation_data make_from_start_end_delta(
      double first_point, double desired_delta, double last_point);

  Interpolation_data make_from_start_number_end(
      double first_point, double last_point, int number_of_entries);

  class InterpolatingVector {
  public:
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

    Eigen::Ref<Eigen::VectorXd const> const_timestep(Eigen::Index index) const;

  private:
    std::vector<double> const interpolation_points;
    Eigen::Index const inner_length;
    Eigen::VectorXd allvalues;
  };
} // namespace Aux
