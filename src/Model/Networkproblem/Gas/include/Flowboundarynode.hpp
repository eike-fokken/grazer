#pragma once
#include "Boundaryvaluecomponent.hpp"
#include "Equationcomponent.hpp"
#include "Gasnode.hpp"
#include "InterpolatingVector.hpp"
#include "Node.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Gas {

  class Flowboundarynode : public Gasnode, public Boundaryvaluecomponent {

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

    void d_evalutate_d_last_state(
        Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
        double /*new_time*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const override;

  private:
    Aux::InterpolatingVector const boundaryvalue;
  };

} // namespace Model::Gas
