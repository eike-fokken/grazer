#pragma once
#include "Valuemap.hpp"
#include "Mathfunctions.hpp"
#include <Eigen/Sparse>
#include "Exception.hpp"
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>

namespace Model::Networkproblem {
  template <typename T, int N> class Boundaryvalue {

  public:
    Boundaryvalue(){};
    Boundaryvalue(
        std::map<double, Eigen::Matrix<double, N, 1>> boundary_values)
        : valuemap(boundary_values){};
	
    void set_boundary_condition(nlohmann::json values_json) {
      valuemap.set_condition(values_json,"time");
    };

    Eigen::Matrix<double, N, 1> operator()(double t) const {
      try {
        return valuemap(t);
      } catch (Valuemap_out_of_range &e) {
        gthrow({"Out-of-range error in boundary values!\n",std::string(e.what())});
      }
    }
    

  private:
    Valuemap<N> valuemap;

  }; // namespace Model::Networkproblem
}
