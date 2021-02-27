#pragma once
#include "Gasnode.hpp"
#include "Bernoulligasnode.hpp"
#include <Boundaryvalue.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace Aux {
  class Matrixhandler;
}

namespace Model::Networkproblem::Gas {

  template <typename Couplingnode>
  class Flowboundarynode: public Couplingnode {

public:


    Flowboundarynode<Couplingnode>(nlohmann::json const & data);

    ~Flowboundarynode<Couplingnode>() override {};

  
    void evaluate(Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
                  double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
                  Eigen::Ref<Eigen::VectorXd const> const &new_state) const override final;
    void evaluate_state_derivative(
        Aux::Matrixhandler * jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &, Eigen::Ref<Eigen::VectorXd const> const &new_state) const override final;

    void display() const override;


private:
  
  Boundaryvalue<Flowboundarynode<Couplingnode>, 1> boundaryvalue;
  };

  extern template class Flowboundarynode<Gasnode>;
  extern template class Flowboundarynode<Bernoulligasnode>;

  using Pressurecouplingnode = Flowboundarynode<Gasnode>;
  using Bernoullicouplingnode = Flowboundarynode<Bernoulligasnode>;

} // namespace Model::Networkproblem::Gas
