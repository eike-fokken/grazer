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
#include "Boundaryvaluecomponent.hpp"
#include "Equationcomponent.hpp"
#include "Gasnode.hpp"
#include "InterpolatingVector.hpp"
#include "Node.hpp"
#include <nlohmann/json.hpp>
#include <string>

namespace Model::Gas {

  nlohmann::json revert_boundary_conditions(nlohmann::json const &data);

  class Flowboundarynode :
      public Equationcomponent,
      public Gasnode,
      public Boundaryvaluecomponent {

  public:
    static nlohmann::json get_boundary_schema();

    Flowboundarynode(nlohmann::json const &data);

    ~Flowboundarynode(){};

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

  private:
    Aux::InterpolatingVector const boundaryvalue;
  };

} // namespace Model::Gas
