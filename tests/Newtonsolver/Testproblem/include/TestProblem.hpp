/*
 * Grazer - network simulation tool
 *
 * Copyright 2020-2021 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	AGPL-3.0-or-later
 *
 * Licensed under the GNU Affero General Public License v3.0, found in
 * LICENSE.txt and at https://www.gnu.org/licenses/agpl-3.0.html
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */

#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <memory>
#include <vector>

namespace Aux {
  class Matrixhandler;
}
/*! \namespace Model
 *  This namespace holds all data for setting up model equations and for taking
 * derivatives thereof.
 */

namespace GrazerTest {

  using rootfunction = Eigen::VectorXd(Eigen::VectorXd);
  using Derivative = Eigen::SparseMatrix<double>(Eigen::VectorXd);

  class TestProblem {

  public:
    /// The constructor needs to declare Delta_t
    ///
    TestProblem(rootfunction _f, Derivative _df);

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double, double,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const;

    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double, double,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const;

    rootfunction *f;
    Derivative *df;
  };
} // namespace GrazerTest
