#include <Boundaryvalue.hpp>
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <iterator>
#include <map>
#include <vector>

namespace Model::Networkproblem {

  Eigen::VectorXd Boundaryvalue::operator()(double t) const{
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

  double t_minus              = previous->first;
  Eigen::VectorXd value_minus = previous->second;
  double t_plus               = next->first;
  Eigen::VectorXd value_plus  = next->second;


  Eigen::VectorXd value = previous->second + (t - t_minus) *
                                                 (value_plus - value_minus) /
                                                 (t_plus - t_minus);
  return value;
}

  Eigen::VectorXd  Boundaryvalue::operator()(double t) {
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

} // namespace Model::Networkproblem
