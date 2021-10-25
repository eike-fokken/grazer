#pragma once
#include "Powernode.hpp"

namespace Model::Power {

  class PQnode final : public Powernode {

  public:
    static char const constexpr *get_type() { return "PQnode"; }

    std::string get_power_type() const override;
    using Powernode::Powernode;

    /// In this node  evaluate the equation
    /// for P and Q.
    virtual void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;

    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const override;

    void d_evalutate_d_last_state(
        Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
        double /*new_time*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const override;

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) override;
  };

} // namespace Model::Power
