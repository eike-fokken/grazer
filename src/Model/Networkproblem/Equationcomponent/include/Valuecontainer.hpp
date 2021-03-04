#pragma once
#include <map>
#include <vector>

namespace Model::Networkproblem {
  struct Valuecontainer {
    std::vector<double> times;
    std::vector<std::vector<std::map<double, double>>> values;
  };

} // namespace Model::Networkproblem
