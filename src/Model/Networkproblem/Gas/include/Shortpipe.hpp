#pragma once
#include "Shortcomponent.hpp"

namespace Model::Gas {

  class Shortpipe : public Equationcomponent, public Shortcomponent {

  public:
    static std::string get_type();
    std::string get_gas_type() const override;
    using Shortcomponent::Shortcomponent;

    void setup() override;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void add_results_to_json(nlohmann::json &new_output) override;
  };
} // namespace Model::Gas
