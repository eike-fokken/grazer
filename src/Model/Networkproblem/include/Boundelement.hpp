#include <Eigen/Sparse>
#include <map>
#include <vector>

namespace Model::Networkproblem {
class Boundelement {

  Boundelement(std::map<double, Eigen::VectorXd> _boundary_values)
      : boundary_values(_boundary_values){};

  Eigen::VectorXd get_boundaryvalue(double t);

private:
  std::map<double, Eigen::VectorXd> boundary_values;
};

} // namespace Model::Networkproblem
