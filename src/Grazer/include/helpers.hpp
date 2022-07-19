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
#include <filesystem>
#include <nlohmann/json.hpp>
namespace Aux {
  class InterpolatingVector;
}
namespace Model {
  class OptimizableObject;
  struct Timedata;
} // namespace Model

void setup_controls(
    Aux::InterpolatingVector &controls, Model::OptimizableObject &problem,
    Model::Timedata &timedata, nlohmann::json const &all_json,
    std::filesystem::path const &problem_directory);
