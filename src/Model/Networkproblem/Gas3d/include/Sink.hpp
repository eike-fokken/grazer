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
#include "Flowboundarynode.hpp"

namespace Model::Gas3d {

  nlohmann::json revert_first_boundary_conditions(nlohmann::json const &data);

  class Sink final : public Flowboundarynode {

  public:
    Sink(nlohmann::json const &data);

    static std::string get_type();

    static nlohmann::json get_boundary_schema();

  private:
    Aux::InterpolatingVector const boundaryvalue;

    double prescribed_flow_value(double time) const final;
    double prescribed_component_1_share(double time) const final;
  };

} // namespace Model::Gas3d
