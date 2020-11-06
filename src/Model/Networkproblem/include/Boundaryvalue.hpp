#pragma once
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <map>
#include <vector>

namespace Model::Networkproblem {
template <typename T, int N> class Boundaryvalue {

public:
  Boundaryvalue(std::map<double, Eigen::Matrix<double, N, 1>> _boundary_values)
      : boundary_values(_boundary_values){};

  Eigen::VectorXd operator()(double t) const {
    auto next = boundary_values.lower_bound(t);

    auto previous = std::prev(next);
    if (next == boundary_values.end()) {
      gthrow(
          "Requested boundary value is at a later time than the given values.");
    }
    if (next == previous) {
      gthrow("Requested boundary value is at an earlier time than the given "
             "values.");
    }

    double t_minus = previous->first;
    Eigen::VectorXd value_minus = previous->second;
    double t_plus = next->first;
    Eigen::VectorXd value_plus = next->second;

    Eigen::VectorXd value = previous->second + (t - t_minus) *
                                                   (value_plus - value_minus) /
                                                   (t_plus - t_minus);
    return value;
  }
  Eigen::VectorXd operator()(double t) {
    auto next = boundary_values.lower_bound(t);

    auto previous = std::prev(next);
    if (next == boundary_values.end()) {
      gthrow(
          "Requested boundary value is at a later time than the given values.");
    }
    if (next == previous) {
      gthrow("Requested boundary value is at an earlier time than the given "
             "values.");
    }

    double t_minus = previous->first;
    Eigen::VectorXd value_minus = previous->second;
    double t_plus = next->first;
    Eigen::VectorXd value_plus = next->second;

    Eigen::VectorXd value = previous->second + (t - t_minus) *
                                                   (value_plus - value_minus) /
                                                   (t_plus - t_minus);
    return value;
  }

private:
  std::map<double, Eigen::Matrix<double, N, 1>> boundary_values;
};

} // namespace Model::Networkproblem
