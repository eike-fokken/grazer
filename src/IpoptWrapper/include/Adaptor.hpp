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

#include "Wrapper.hpp"

#include <Eigen/Dense>
#include <IpIpoptApplication.hpp>
#include <memory>

namespace Optimization {
  class Optimizer;

  class IpoptAdaptor {
  private:
    Ipopt::SmartPtr<Optimization::IpoptWrapper> _nlp;
    Ipopt::SmartPtr<Ipopt::IpoptApplication> _app;

  public:
    IpoptAdaptor(std::unique_ptr<Optimizer> optimizer);
    ~IpoptAdaptor() = default;

    Ipopt::ApplicationReturnStatus optimize() const;

    Eigen::VectorXd get_solution() const;
    double get_obj_value() const;
  };
} // namespace Optimization
