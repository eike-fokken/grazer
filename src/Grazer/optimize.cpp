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

// auto lower_bounds_json = aux_json::get_json_from_file_path(
//     problem_directory / std::filesystem::path("lower_bounds.json"));
// auto upper_bounds_json = aux_json::get_json_from_file_path(
//     problem_directory / std::filesystem::path("upper_bounds.json"));
// auto constraints_lower_bounds_json = aux_json::get_json_from_file_path(
//     problem_directory
//     / std::filesystem::path("constraints_lower_bounds.json"));
// auto constraints_upper_bounds_json = aux_json::get_json_from_file_path(
//     problem_directory
//     / std::filesystem::path("constraints_upper_bounds.json"));

// Aux::InterpolatingVector lower_bounds(
//     control_timehelper, problem.get_number_of_controls_per_timepoint());
// Aux::InterpolatingVector upper_bounds(
//     control_timehelper, problem.get_number_of_controls_per_timepoint());
// Aux::InterpolatingVector constraints_lower_bounds(
//     control_timehelper,
//     problem.get_number_of_constraints_per_timepoint());
// Aux::InterpolatingVector constraints_upper_bounds(
//     control_timehelper,
//     problem.get_number_of_constraints_per_timepoint());

// Optimization::initialize(
//     problem, controller, control_json, initial_state, initial_json,
//     lower_bounds, lower_bounds_json, upper_bounds, upper_bounds_json,
//     constraints_lower_bounds, constraints_lower_bounds_json,
//     constraints_upper_bounds, constraints_upper_bounds_json);
