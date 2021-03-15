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
  template <int N> class Boundaryvalue {

  public:
    Boundaryvalue(nlohmann::json const &boundary_json):
      valuemap(boundary_json,"time"){};
	

    Eigen::Matrix<double, N, 1> operator()(double t) const {
      try {
        return valuemap(t);
      } catch (Valuemap_out_of_range &e) {
        gthrow({"Out-of-range error in boundary values!\n",std::string(e.what())});
      }
    }
    

  private:
    Valuemap<N> const valuemap;

  }; // namespace Model::Networkproblem
}
