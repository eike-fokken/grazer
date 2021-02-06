#pragma once
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>

namespace Model::Networkproblem {
  template <typename T, int N> class Boundaryvalue {

  public:
    Boundaryvalue(){};
    Boundaryvalue(
        std::map<double, Eigen::Matrix<double, N, 1>> _boundary_values)
        : boundary_values(_boundary_values){};

    Eigen::VectorXd operator()(double t) const {
      auto next = boundary_values.lower_bound(t);

      auto previous = std::prev(next);
      if (next == boundary_values.end()) {
        gthrow({"Requested boundary value is at a later time than the "
                "given values."});
      }
      if (next == boundary_values.begin()) {
        gthrow({"Requested boundary value is at an earlier time than the given "
                "values."});
      }

      if (next->first == t) {
        return next->second;
      }

      double t_minus = previous->first;
      Eigen::VectorXd value_minus = previous->second;
      double t_plus = next->first;
      Eigen::VectorXd value_plus = next->second;

      Eigen::VectorXd value =
          previous->second +
          (t - t_minus) * (value_plus - value_minus) / (t_plus - t_minus);
      return value;
    }
    // Eigen::VectorXd operator()(double t) {
    //   auto next = boundary_values.lower_bound(t);
    //   if (next->first == t) {
    //     return next->second;
    //   }

    //   auto previous = std::prev(next);
    //   if (next == boundary_values.end()) {
    //     gthrow({"Requested boundary value is at a later time than the given "
    //             "values."});
    //   }
    //   if (next == boundary_values.begin()) {
    //     gthrow({"Requested boundary value is at an earlier time than the
    //     given "
    //             "values."});
    //   }

    //   double t_minus = previous->first;
    //   Eigen::VectorXd value_minus = previous->second;
    //   double t_plus = next->first;
    //   Eigen::VectorXd value_plus = next->second;

    //   Eigen::VectorXd value =
    //       previous->second +
    //       (t - t_minus) * (value_plus - value_minus) / (t_plus - t_minus);
    //   return value;
    // }

    void set_boundary_condition(nlohmann::ordered_json boundary_json) {
    
      for (auto &datapoint : boundary_json["data"]) {
        if (datapoint["values"].size() != N) {
          gthrow(
                 {"Wrong number of boundary values in node ", boundary_json["id"]});
        }
        Eigen::Matrix<double, N, 1> value;
        try {
          for (unsigned int i =0 ; i<N ; ++i) {
            // auto ijson= static_cast<nlohmann::basic_json::size_type>(i);
          value[i] = datapoint["values"][i];
          }
        } catch (...) {
          gthrow({"Boundary data in node with id ", boundary_json["id"],
                  " couldn't be assignd in vector, not a double?"})
            }
        boundary_values.insert({datapoint["time"], value});
      }
    }


  private:
    std::map<double, Eigen::Matrix<double, N, 1>> boundary_values;
  };

} // namespace Model::Networkproblem
