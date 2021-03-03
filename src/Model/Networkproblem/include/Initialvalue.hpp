#pragma once
#include "Valuemap.hpp"
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <map>
#include <stdexcept>
#include <vector>
#include <nlohmann/json.hpp>
#include <Mathfunctions.hpp>

namespace Model::Networkproblem {
  template <typename T, int N> class Initialvalue  {

   public:
    Initialvalue(){};
    Initialvalue(
        std::map<double, Eigen::Matrix<double, N, 1>> _initial_values)
        : valuemap(_initial_values){};

    void set_initial_condition(nlohmann::json values_json) {
      valuemap.set_condition(values_json);
        };

        Eigen::Matrix<double, N, 1> operator()(double t) const {
          try {
            return valuemap(t);
          } catch (Valuemap_out_of_range &e) {
            gthrow({"Out-of-range error in initial values!\n",
                    std::string(e.what())});
          }
        }


      private:
	Valuemap<N> valuemap;
  };

} // namespace Model::Networkproblem
