#pragma once
#include "Valuemap.hpp"
#include <Mathfunctions.hpp>
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>

namespace Model::Networkproblem {
  template <typename T, int N> class Boundaryvalue {

  public:
    Boundaryvalue(){};
    Boundaryvalue(
        std::map<double, Eigen::Matrix<double, N, 1>> _boundary_values)
        : boundaryvalues(_boundary_values){};
	
    

    //Eigen::VectorXd operator()(double t) const {
      
    //  Note that by virtue of the function set_boundary condition, this map is never empty!
    //  last element of the map:
    //  auto earliest_element = Boualues.rbegin();
    //  auto earliest_time = boundary_values.rbegin()->first;
    //  first element of the map:
    //  auto last_element = boundary_values.begin();
    //  auto last_time = boundary_values.begin()->first;

      // This catches problems with doubles

    //  if (t >= latest_time) {
    //    if (t > latest_time + Aux::EPSILON) {
    //      std::cout << "Demanded time: " << t
    //                << ", latest possible time: " << latest_time << std::endl;
    //      gthrow({"Requested boundary value is at a later time than the given "
    //          "values."});
    //    } else {
    //      return latest_element->second;
    //    }
    //  }
    //  if (t <= earliest_time) {
    //    if (t < earliest_time-Aux::EPSILON) {
    //      std::cout << "Demanded time: " << t << ", earliest possible time: " << earliest_time << std::endl;
    //      gthrow({"Requested boundary value is at an earlier time than the given "
    //          "values."});
    //    } else {
    //      return earliest_element->second;
    //    }
    //  }
      
            

    //  auto next = boundary_values.lower_bound(t);
    //  auto previous = std::prev(next);

    //  double t_minus = previous->first;
    //  Eigen::VectorXd value_minus = previous->second;
    //  double t_plus = next->first;
    //  Eigen::VectorXd value_plus = next->second;

    //  Eigen::VectorXd value =
    //      previous->second +
    //      (t - t_minus) * (value_plus - value_minus) / (t_plus - t_minus);
    //  return value;
    // }


   //void set_boundary_condition(nlohmann::ordered_json boundary_json) {
   //  if(boundary_json.size()==0) {
   //    gthrow({"Boundary data in node with id ", boundary_json["id"],
   //            " is empty!"})

   //  }

   //   for (auto &datapoint : boundary_json["data"]) {
   //     if (datapoint["values"].size() != N) {
   //       gthrow(
   //              {"Wrong number of boundary values in node ", boundary_json["id"]});
   //     }
   //     Eigen::Matrix<double, N, 1> value;
   //     try {
   //       for (unsigned int i =0 ; i<N ; ++i) {
   //         // auto ijson= static_cast<nlohmann::basic_json::size_type>(i);
   //       value[i] = datapoint["values"][i];
   //       }
   //     } catch (...) {
   //       gthrow({"Boundary data in node with id ", boundary_json["id"],
   //               " couldn't be assignd in vector, not a double?"})
   //         }
   //     boundary_values.insert({datapoint["time"], value});
   //   }
   //}

  private:
    Valuemap<N> boundaryvalues;

	/*
	std::string String1 = "time: ";
	std::string String2 = ", latest possible time: ";
	std::string String3 = "Requested boundary value is at a later time than the given values.";
	std::string String4 = ", ealiest possible time: ";
	std::string String5 = "Requested boundary value is at an ealier time than the given values.";
	*/
	
} // namespace Model::Networkproblem
