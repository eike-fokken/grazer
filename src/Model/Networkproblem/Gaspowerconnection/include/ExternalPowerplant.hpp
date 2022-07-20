/*
 * Grazer - network simulation and optimization tool
 *
 * Copyright 2020-2022 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	MIT
 *
 * Licensed under the MIT License, found in the file LICENSE and at
 * https://opensource.org/licenses/MIT
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */
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
    std::string get_power_type() const final;

    using Powernode::Powernode;

    void setup() final;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void d_evaluate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void d_evaluate_d_last_state(
        Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
        double /*new_time*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const final;

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) final;

  private:
    Gaspowerconnection *connection{nullptr};
  };
} // namespace Model::Gaspowerconnection
