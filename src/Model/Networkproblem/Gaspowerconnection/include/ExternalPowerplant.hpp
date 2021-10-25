#pragma once
#include "Powernode.hpp"

namespace Model::Gaspowerconnection {
  class Gaspowerconnection;

  /** \brief A Powernode to be connected to an edge that leads out of the power
   * network.
   *
   */
  class ExternalPowerplant final : public Power::Powernode {

  public:
    static std::string get_type();
    std::string get_power_type() const override;

    using Powernode::Powernode;

    void setup() override;

    void evaluate(
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

  private:
    Gaspowerconnection *connection{nullptr};
  };
} // namespace Model::Gaspowerconnection
