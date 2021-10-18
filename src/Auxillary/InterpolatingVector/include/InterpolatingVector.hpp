#pragma once
#include "Timedata.hpp"
#include <Eigen/Sparse>
#include <cstddef>
#include <vector>

namespace Aux {

  struct Interpolation_data {
    double first_point;
    double delta;
    size_t number_of_entries;
  };
  Interpolation_data interpolation_points_helper(
      double first_point, double delta, int number_of_entries);

  Interpolation_data interpolation_points_helper(
      double first_point, double desired_delta, double last_point);

  class InterpolatingVector {
  public:
    static nlohmann::json get_schema(size_t number_of_values_per_point);

    InterpolatingVector(Interpolation_data data, Eigen::Index _inner_length);
    InterpolatingVector(
        std::vector<double> interpolation_points, Eigen::Index inner_length);

    static InterpolatingVector construct_from_json(nlohmann::json const &json);
    static InterpolatingVector
    construct_from_json(Interpolation_data data, nlohmann::json const &json);
    static InterpolatingVector construct_from_json(
        std::vector<double> interpolation_points, nlohmann::json const &json);

    void set_values_in_bulk(Eigen::Ref<Eigen::VectorXd> values);

    Eigen::Index get_total_number_of_values() const;
    std::vector<double> const &get_interpolation_points() const;

    Eigen::VectorXd operator()(double time) const;

    Eigen::Ref<Eigen::VectorXd const> const get_allvalues() const;

    Eigen::Ref<Eigen::VectorXd> mut_timestep(Eigen::Index index);

  private:
    std::vector<double> const interpolation_points;
    Eigen::Index const inner_length;
    Eigen::VectorXd allvalues;
  };
} // namespace Aux
