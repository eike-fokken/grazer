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
#include "Valuemap.hpp"
#include <Eigen/Sparse>
#include <Exception.hpp>
#include <Mathfunctions.hpp>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>

namespace Model::Networkproblem {
  template <int N> class Initialvalue {

  public:
    Initialvalue(nlohmann::json const &initial_json) :
        valuemap(initial_json, "x"){};

    // std::map<double, Eigen::Matrix<double, N, 1>> set_initial_condition(
    // nlohmann::json values_json) {
    // return Valuemap<N>::set_condition(values_json,"x");
    // };

    Eigen::Matrix<double, N, 1> operator()(double t) const {
      try {
        return valuemap(t);
      } catch (Valuemap_out_of_range &e) {
        gthrow(
            {"Out-of-range error in initial values!\n", std::string(e.what())});
      }
    }

  private:
    Valuemap<N> const valuemap;
  };

} // namespace Model::Networkproblem
