#include <Eigen/Sparse>
#include <map>
#include <vector>

namespace Model::Networkproblem {
class Boundaryvalue {

  Boundaryvalue(std::map<double, Eigen::VectorXd> _boundary_values)
      : boundary_values(_boundary_values){};

  Eigen::VectorXd operator()(double t) const;
  Eigen::VectorXd operator()(double t);

private:
  std::map<double, Eigen::VectorXd> boundary_values;
};

} // namespace Model::Networkproblem
