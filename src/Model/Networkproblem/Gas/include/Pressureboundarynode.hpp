#include <Boundaryvalue.hpp>
#include <Equationcomponent.hpp>
#include <Gasnode.hpp>
#include <Node.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Networkproblem::Gas {

  class Pressureboundarynode final: public Gasnode {

public:
    Pressureboundarynode(nlohmann::json const &data);

    ~Pressureboundarynode() override{};

  void
  evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
           double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
           Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;
  void evaluate_state_derivative(
      Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
      Eigen::Ref<Eigen::VectorXd const> const &,
      Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;


private:
  Boundaryvalue<1> const boundaryvalue;
  };

} // namespace Model::Networkproblem::Gas
