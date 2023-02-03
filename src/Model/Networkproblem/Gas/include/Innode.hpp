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
#include <Gasnode.hpp>

namespace Model::Gas {

  /// This class represents an inner node of the gas network, that neither
  /// injects gas nor takes gas out.
  class Innode final : public Equationcomponent, public Gasnode {

  public:
    static std::string get_type();

    // Maybe we should incorporate the min and max pressure later on...
    using Gasnode::Gasnode;

    virtual ~Innode(){};

    void setup() final;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const final;
    void d_evaluate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const final;

    void d_evaluate_d_last_state(
        Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
        double /*new_time*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const final;
  };

} // namespace Model::Gas
