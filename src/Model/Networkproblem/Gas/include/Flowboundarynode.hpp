#pragma once
#include <Boundaryvalue.hpp>
#include <Equationcomponent.hpp>
#include <Gasnode.hpp>
#include <Node.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Networkproblem::Gas {

  class Flowboundarynode : public Gasnode {

  public:
    static std::optional<nlohmann::json> get_boundary_schema();

    Flowboundarynode(nlohmann::json const &data);

    ~Flowboundarynode() override{};

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state)
        const override final;
    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state)
        const override final;

  private:
    Boundaryvalue<1> const boundaryvalue;
  };

} // namespace Model::Networkproblem::Gas
