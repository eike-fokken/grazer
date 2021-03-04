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
  template <typename T, int N> class Control  {

   public:
    Control(){};
    Control(
        std::map<double, Eigen::Matrix<double, N, 1>> _controls)
        : valuemap(_controls){};

    void set_controls(nlohmann::json values_json) {
      valuemap.set_condition(values_json,"time");
        };

        Eigen::Matrix<double, N, 1> operator()(double t) const {
          try {
            return valuemap(t);
          } catch (Valuemap_out_of_range &e) {
            gthrow({"Out-of-range error in control values!\n",
                    std::string(e.what())});
          }
        }


      private:
	Valuemap<N> valuemap;
  };

} // namespace Model::Networkproblem
