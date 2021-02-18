#pragma once
#include "Valuemap.hpp"
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>
#include <Mathfunctions.hpp>

namespace Model::Networkproblem {
  template <typename T, int N> class Initialvalue {

   public:
    Initialvalue(){};
    Initialvalue(
        std::map<double, Eigen::Matrix<double, N, 1>> _initial_values)
        : initialvalues(_initial_values){};

    /*
    Eigen::VectorXd operator()(double t) const {
      auto next = initial_values.lower_bound(t);

      if (next->first == t) {
        return next->second;
      }

      auto previous = std::prev(next);
      if (next == initial_values.end()) {
        gthrow({"Requested initial value is at a greater position than provided."});
      }
      if (next == initial_values.begin()) {
        gthrow({"Requested initial value is at a negative position."});
      }

      double t_minus = previous->first;
      Eigen::VectorXd value_minus = previous->second;
      double t_plus = next->first;
      Eigen::VectorXd value_plus = next->second;

      Eigen::VectorXd value =
          previous->second +
          (t - t_minus) * (value_plus - value_minus) / (t_plus - t_minus);
      return value;
    } */

    // Eigen::VectorXd operator()(double t) {
    //   auto next = initial_values.lower_bound(t);
    //   if (next->first == t) {
    //     return next->second;
    //   }

    //   auto previous = std::prev(next);
    //   if (next == initial_values.end()) {
    //     gthrow({"Requested initial value is at a later time than the given "
    //             "values."});
    //   }
    //   if (next == initial_values.begin()) {
    //     gthrow({"Requested initial value is at an earlier time than the
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


    /// This just makes a map of initial conditions.
    /// The last value is shifted an epsilon to the right in order to circumvent double-equality problems.
    //void set_initial_condition(nlohmann::ordered_json initial_json) {
    //
    //  for (auto &datapoint : initial_json["data"]) {
    //    if (datapoint["value"].size() != N) {
    //      gthrow(
    //             {"Wrong number of initial values in object ", initial_json["id"]});
    //    }
        
    //    Eigen::Matrix<double, N, 1> value;
    //    try {
    //      for (unsigned int i =0 ; i<N ; ++i) {
    //        // auto ijson= static_cast<nlohmann::basic_json::size_type>(i);
    //      value[i] = datapoint["value"][i];
    //      }
    //    } catch (...) {
    //      gthrow({"Initial data in object with id ", initial_json["id"],
    //              " couldn't be assignd in vector, not a double?"});
    //    }
    //    if(datapoint == initial_json["data"].back()){
    //      initial_values.insert({datapoint["x"].get<double>()+Aux::EPSILON, value});
    //    }
    //    initial_values.insert({datapoint["x"], value});
    //    
    //  }

    //}
    private:
	Valuemap<N> initialvalues;
  };

} // namespace Model::Networkproblem
