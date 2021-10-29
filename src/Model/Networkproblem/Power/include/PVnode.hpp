#pragma once
#include <Powernode.hpp>

namespace Model::Power {

  class PVnode final : public Powernode {

  public:
    static std::string get_type();
    std::string get_power_type() const final;
    using Powernode::Powernode;

    void setup() final;

    /**
     * @brief  In this node we set V to its boundary value and evaluate the
     * equation for P.
     *
     * @param    rootvalues           TODO
     * @param    last_time            TODO
     * @param    new_time             TODO
     * @param    last_state           TODO
     * @param    new_state            TODO
     */
    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void d_evalutate_d_last_state(
        Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
        double /*new_time*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const final;

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) final;
  };

} // namespace Model::Power
