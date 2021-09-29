#pragma once
#include "Shortcomponent.hpp"

namespace Model::Networkproblem::Gas {

  class Shortpipe : public Shortcomponent {

  public:
    static std::string get_type();
    std::string get_gas_type() const override;
    using Shortcomponent::Shortcomponent;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &last_control,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const final;

    void d_evalutate_d_new_state(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &last_control,
        Eigen::Ref<Eigen::VectorXd const> const &new_control) const final;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) final;

    void add_results_to_json(nlohmann::json &new_output) override;
  };
} // namespace Model::Networkproblem::Gas
