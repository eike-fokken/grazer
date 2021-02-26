#pragma once
#include "Mathfunctions.hpp"
#include <Eigen/Sparse>
#include "Exception.hpp"
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>

namespace Model::Networkproblem {
  template <typename T, int N> class Control {

  public:
    Control(){};
    Control(
        std::map<double, Eigen::Matrix<double, N, 1>> _control_values)
        : control_values(_control_values){};

    Eigen::VectorXd operator()(double t) const {
      
      //Note that by virtue of the function set_controls, this map is never empty!
      // last element of the map:
      auto latest_element = control_values.rbegin();
      auto latest_time = control_values.rbegin()->first;
      // first element of the map:
      auto earliest_element = control_values.begin();
      auto earliest_time = control_values.begin()->first;

      // This catches problems with doubles
      if (t >= latest_time) {
        if (t > latest_time + Aux::EPSILON) {
          std::cout << "Demanded time: " << t
                    << ", latest possible time: " << latest_time << std::endl;
          gthrow({"Requested control value is at a later time than the given "
              "values."});
        } else {
          return latest_element->second;
        }
      }
      if (t <= earliest_time) {
        if (t < earliest_time-Aux::EPSILON) {
          std::cout << "Demanded time: " << t << ", earliest possible time: " << earliest_time << std::endl;
          gthrow({"Requested control value is at a later time than the given "
              "values."});
        } else {
          return earliest_element->second;
        }
      }
      
            

      auto next = control_values.lower_bound(t);
      auto previous = std::prev(next);

      double t_minus = previous->first;
      Eigen::VectorXd value_minus = previous->second;
      double t_plus = next->first;
      Eigen::VectorXd value_plus = next->second;

      Eigen::VectorXd value =
          previous->second +
          (t - t_minus) * (value_plus - value_minus) / (t_plus - t_minus);
      return value;
    }


   void set_controls(nlohmann::ordered_json control_json) {
     if(control_json.size()==0) {
       gthrow({"Control data in node with id ", control_json["id"],
               " is empty!"})

     }

      for (auto &datapoint : control_json) {
        if (datapoint["values"].size() != N) {
          gthrow(
                 {"Wrong number of control values in node ", control_json["id"]});
        }
        Eigen::Matrix<double, N, 1> value;
        try {
          for (unsigned int i =0 ; i<N ; ++i) {
            value[i] = datapoint["values"][i];
          }
        } catch (...) {
          gthrow({"Control data in node with id ", control_json["id"],
                  " couldn't be assignd in vector, not a double?"})
            }
        control_values.insert({datapoint["time"], value});
      }
    }

  private:
    std::map<double, Eigen::Matrix<double, N, 1>> control_values;
  };

} // namespace Model::Networkproblem
