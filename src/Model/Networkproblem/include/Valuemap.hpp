#pragma once
#include "Eigen/Sparse"
#include "Exception.hpp"
#include "Mathfunctions.hpp"

#include <nlohmann/json.hpp>
#include <iostream>
#include <map>
#include <vector>

namespace Model::Networkproblem {
  template <int N> class Valuemap {

  public:
    Valuemap(){};
    Valuemap(
        std::map<double, Eigen::Matrix<double, N, 1>> values)
        : Values(values){};

    Eigen::VectorXd operator()(double t) const {
      
      //Note that by virtue of the function set_boundary condition, this map is never empty!
      // last element of the map:
      auto last_element = Values.rbegin();
      auto last_t = Values.rbegin()->first;
      // first element of the map:
      auto first_element = Values.begin();
      auto first_t = Values.begin()->first;

      // This catches problems with doubles
      try {
	if (t >= last_t) {
          if (t > last_t + Aux::EPSILON) {
            throw std::out_of_range("Too high");
          } else {
            return last_element->second;
          }
        }
        if (t <= first_t) {
          if (t < first_t-Aux::EPSILON) {
	    throw std::out_of_range("Too low");
          } else {
            return first_element->second;
          }
        }
      } catch (std::out_of_range& e) {
        std::cout << "The value " << t
		  << "is out of range of the initial/boundary values. The valid range is between "
		  << first_t << " and " << last_t << "." << std::endl;
      }       
            
      auto next = Values.lower_bound(t);
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


   void set_condition(nlohmann::json values_json) {
     if(values_json.size()==0) {
       gthrow({"data in node with id ", values_json["id"],
               " is empty!"})

     }

      for (auto &datapoint : values_json["data"]) {
        if (datapoint["values"].size() != N) {
          gthrow(
                 {"Wrong number of initial/boundary values in node ", values_json["id"]});
        }
        Eigen::Matrix<double, N, 1> value;
        try {
          for (unsigned int i =0 ; i<N ; ++i) {
            // auto ijson= static_cast<nlohmann::basic_json::size_type>(i);
          value[i] = datapoint["values"][i];
          }
        } catch (...) {
          gthrow({"initial/boundary data in node with id ",values_json["id"],
                  " couldn't be assignd in vector, not a double?"})
            }
        Values.insert({datapoint["time"], value});
      }
    }

    private:
	std::map<double, Eigen::Matrix<double, N, 1>> Values;
    };

} // namespace Model::Networkproblem
