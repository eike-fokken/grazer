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
#include "Subproblem.hpp"
#include "gmock/gmock.h"
#include <Eigen/Sparse>
#include <nlohmann/json.hpp>
#include <string>

namespace GrazerTest {

  class MockSubproblem : public Model::Subproblem {

  public:
    MOCK_METHOD((std::string), get_type, (), (override, const));
    MOCK_METHOD(
        void, evaluate,
        ((Eigen::Ref<Eigen::VectorXd>), (double), (double),
         (Eigen::Ref<Eigen::VectorXd const>),
         (Eigen::Ref<Eigen::VectorXd const>)),
        (const, override));
    MOCK_METHOD(
        void, prepare_timestep,
        ((double), (double), (Eigen::Ref<Eigen::VectorXd const>),
         (Eigen::Ref<Eigen::VectorXd const>)),
        (override));

    MOCK_METHOD(
        void, evaluate_state_derivative,
        ((Aux::Matrixhandler *), (double), (double),
         (Eigen::Ref<Eigen::VectorXd const> last_state),
         (Eigen::Ref<Eigen::VectorXd const> new_state)),
        (const, override));
    MOCK_METHOD(int, reserve_indices, (int const next_free_index), (override));
    MOCK_METHOD(
        (void), set_initial_values,
        (Eigen::Ref<Eigen::VectorXd>, nlohmann::json), (override));
    MOCK_METHOD(
        (void), json_save, (double, Eigen::Ref<Eigen::VectorXd const>),
        (override));
    MOCK_METHOD((void), print_to_files, (nlohmann::json &), (override));
  };
} // namespace GrazerTest
