#pragma once
#include "Eigen/Sparse"
#include "Exception.hpp"
#include "Mathfunctions.hpp"

#include <nlohmann/json.hpp>
#include <iostream>
#include <map>
#include <stdexcept>
#include <vector>

namespace Model::Networkproblem {

  /// \brief This exception class returns information on whether too high or too low a value was requested.
  ///
  /// The returned data also indicates the maximal (or minimal) valid argument.

    

  class Valuemap_out_of_range : public std::out_of_range {
  public:
    Valuemap_out_of_range(std::string message, bool too_high, double requested_argument, double
                          boundary_valid_argument)
        : std::out_of_range(message)
    {
      std::ostringstream o;
      if(too_high) {
        o << "Requested value " << requested_argument
          << " is higher than the last valid value: " << boundary_valid_argument
          << "\n";
      } else {
        o << "Requested value " << requested_argument
          << " is lower than the first valid value: " << boundary_valid_argument
          << "\n";
      }

      msg = o.str();
    }

    /// -1 means too low, 1 means too high
    std::string msg;
    char const *what() const noexcept { return msg.c_str(); }
    
  };


  template <int N>
  class Valuemap {

  public:
    Valuemap(){};
    Valuemap(
        std::map<double, Eigen::Matrix<double, N, 1>> values)
        : Values(values){};

    Eigen::Matrix<double, N, 1> operator()(double t) const {

      //Note that by virtue of the function set_condition, this map is never empty!
      // last element of the map:
      auto last_element = Values.rbegin();
      auto last_t = Values.rbegin()->first;
      // first element of the map:
      auto first_element = Values.begin();
      auto first_t = Values.begin()->first;

      // This catches problems with doubles
      if (t >= last_t) {
        if (t > last_t + Aux::EPSILON) {
          throw Valuemap_out_of_range("too high", 1, t, last_t);
        }

        return last_element->second;
      }
      if (t <= first_t) {
        if (t < first_t - Aux::EPSILON) {
          throw Valuemap_out_of_range("too low", -1, t, first_t);
        }
        
        return first_element->second;
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

    void set_condition(nlohmann::json values_json, std::string const & key) {
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
        Values.insert({datapoint[key], value});
      }
    }

    private:
	std::map<double, Eigen::Matrix<double, N, 1>> Values;
  };

} // namespace Model::Networkproblem