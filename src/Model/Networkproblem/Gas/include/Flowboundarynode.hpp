#pragma once
#include <Boundaryvalue.hpp>
#include <Equationcomponent.hpp>
#include <Gasnode.hpp>
#include <Node.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Networkproblem::Gas {

class Flowboundarynode: public Gasnode {

public:


  Flowboundarynode(nlohmann::json const & data);

  ~Flowboundarynode() override {};

  
    void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                  double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                  Eigen::Ref<Eigen::VectorXd const> const &new_state) const override final;
    void evaluate_state_derivative(
        Aux::Matrixhandler * jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &, Eigen::Ref<Eigen::VectorXd const> const &new_state) const override final;

    void display() const override;


private:
  
  Boundaryvalue<Flowboundarynode, 1> boundaryvalue;
  };

} // namespace Model::Networkproblem::Gas
