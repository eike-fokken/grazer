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
#include "Adaptor.hpp"
#include "InterpolatingVector.hpp"
#include "OptimizableObject.hpp"
#include "Optimizer.hpp"
#include <memory>

namespace Optimization {

  IpoptAdaptor::IpoptAdaptor(std::unique_ptr<Optimizer> optimizer) :
      _nlp(new Optimization::IpoptWrapper(std::move(optimizer))),
      _app(IpoptApplicationFactory()) {}

  Ipopt::ApplicationReturnStatus IpoptAdaptor::optimize() const {
    // Approximate the hessian
    _app->Options()->SetStringValue("hessian_approximation", "limited-memory");
    // disable Ipopt's console output
    _app->Options()->SetIntegerValue("print_level", 5);
    // Supress Ipopt Banner
    _app->Options()->SetBoolValue("sb", true);
    // _app->Options()->SetStringValue("derivative_test", "first-order");
    // Process the options
    auto status = _app->Initialize();
    if (status != Ipopt::Solve_Succeeded) {
      std::cout << "Couldn't find an optimum!" << std::endl;
      return status;
    }
    // solve the problem
    status = _app->OptimizeTNLP(_nlp);
    return status;
  }
  Eigen::VectorXd IpoptAdaptor::get_solution() const {
    return _nlp->get_best_solution();
  }
  double IpoptAdaptor::get_objective_value() const {
    return _nlp->get_best_objective_value();
  }
  double IpoptAdaptor::get_cost_value() const {
    return _nlp->get_best_cost_value();
  }
  double IpoptAdaptor::get_penalty_value() const {
    return _nlp->get_best_penalty_value();
  }
} // namespace Optimization
