#include <Gasnode.hpp>


namespace Model::Networkproblem::Gas {


  /// This class represents an inner node of the gas network, that neither injects gas nor takes gas out.
class Innode final: public Gasnode {

public:

  // Maybe we should incorporate the min and max pressure later on...
  using Gasnode::Gasnode;
  
  virtual ~Innode() {};
  
  void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;
  void evaluate_state_derivative(Aux::Matrixhandler * jacobianhandler, double last_time, double new_time,
                                 Eigen::Ref<Eigen::VectorXd const> const &, Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;

  void display() const override;
};

} // namespace Model::Networkproblem::Gas
