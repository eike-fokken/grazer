#pragma once
#include "Gasedge.hpp"
#include <Boundaryvalue.hpp>
#include <Equationcomponent.hpp>
#include <Gasnode.hpp>
#include <Node.hpp>
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Networkproblem::Gas {

  class Pressureboundarynode : public Gasnode {

  public:
    static std::optional<nlohmann::json> get_boundary_schema();

    Pressureboundarynode(nlohmann::json const &data);

    ~Pressureboundarynode() override{};

    void setup() override;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override final;
    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override final;

  private:
    Boundaryvalue<1> const boundaryvalue;
  };

} // namespace Model::Networkproblem::Gas
