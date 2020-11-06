#include <Exception.hpp>
#include <Powernode.hpp>

#include <Boundaryvalue.hpp>

namespace Model::Networkproblem::Power {

class Vphinode:public Powernode {

public:

  /// In this node we just set V and phi to their respective boundary values.
  virtual void evaluate(Eigen::VectorXd & rootfunction, double current_time, double next_time,
                      const Eigen::VectorXd &current_state,
                      Eigen::VectorXd &new_state) override;

  virtual void evaluate_state_derivative(double current_time, double next_time,
                                       const Eigen::VectorXd &,
                                       Eigen::SparseMatrix<double> &) override;

private:
  Boundaryvalue<Vphinode, 2> boundaryvalue;
};

} // namespace Model::Networkproblem::Power
