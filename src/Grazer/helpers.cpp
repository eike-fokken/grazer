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
#include "helpers.hpp"
#include "Aux_json.hpp"
#include "InterpolatingVector.hpp"
#include "OptimizableObject.hpp"
#include "Timedata.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
void setup_controls(
    Aux::InterpolatingVector &controls, Model::OptimizableObject &problem,
    Model::Timedata &timedata, nlohmann::json const &all_json,
    std::filesystem::path const &problem_directory) {
  if (problem.get_number_of_controls_per_timepoint() > 0) {
    Aux::Interpolation_data control_timehelper;
    if (all_json.contains("control_settings")) {
      auto &control_settings = all_json["control_settings"];
      if (control_settings.contains("desired_delta_t")
          and control_settings.contains("number_of_steps")) {
        std::ostringstream o;
        o << "\"control_settings\" contains both \"desired_delta_t\" "
             "and \"number_of_steps.\". I don't know, what to choose. Please "
             "only provide one.";
        throw std::runtime_error(o.str());
      }
      if (control_settings.contains("desired_delta_t")) {
        auto control_delta_t
            = control_settings["desired_delta_t"].get<double>();
        control_timehelper = Aux::make_from_start_delta_end(
            timedata.get_starttime(), control_delta_t, timedata.get_endtime());
      } else if (control_settings.contains("number_of_steps")) {
        auto number_of_controls
            = control_settings["number_of_steps"].get<int>();
        control_timehelper = Aux::make_from_start_number_end(
            timedata.get_starttime(), timedata.get_endtime(),
            number_of_controls);
      } else {
        std::ostringstream o;
        o << "\"control_settings\" contains neither \"desired_delta_t\" "
             "nor \"number_of_steps.\". I don't know, what to choose. Please "
             "provide one or remove \"control_settings\" alltogether to "
             "choose the simulation times as control times.";
        throw std::runtime_error(o.str());
      }
    } else {
      control_timehelper = Aux::make_from_start_delta_end(
          timedata.get_starttime(), timedata.get_delta_t(),
          timedata.get_endtime());
    }
    auto control_json = aux_json::get_json_from_file_path(
        problem_directory / std::filesystem::path("control.json"));

    controls = Aux::InterpolatingVector(
        control_timehelper, problem.get_number_of_controls_per_timepoint());
    problem.set_initial_controls(controls, control_json);
  }
}
