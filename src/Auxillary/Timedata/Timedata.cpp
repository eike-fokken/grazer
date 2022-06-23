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
#include "Timedata.hpp"
#include <iostream>

namespace Model {

  Timedata::Timedata(nlohmann::json const &simulation_settings) :
      starttime(simulation_settings["start_time"].get<double>()),
      endtime(simulation_settings["end_time"].get<double>()),
      Number_of_timesteps(init_Number_of_timesteps(
          simulation_settings["desired_delta_t"].get<double>())),
      delta_t(init_delta_t()) {}

  int Timedata::init_Number_of_timesteps(double desired_delta_t) const {
    int const _Number_of_timesteps
        = static_cast<int>(std::ceil(get_timeinterval() / desired_delta_t)) + 1;
    return _Number_of_timesteps;
  }

  double Timedata::get_starttime() const { return starttime; }
  double Timedata::get_endtime() const { return endtime; }
  double Timedata::get_delta_t() const { return delta_t; }
  double Timedata::get_timeinterval() const { return endtime - starttime; }
  int Timedata::get_number_of_time_points() const {
    return Number_of_timesteps;
  }

  double Timedata::init_delta_t() const {
    return get_timeinterval() / (Number_of_timesteps - 1);
  }

} // namespace Model
