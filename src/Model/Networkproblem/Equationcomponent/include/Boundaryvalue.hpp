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
#include "Exception.hpp"
#include "Mathfunctions.hpp"
#include "Valuemap.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <vector>

namespace Model::Networkproblem {
  template <int N> class Boundaryvalue {

  public:
    Boundaryvalue(nlohmann::json const &boundary_json) :
        valuemap(boundary_json, "time"){};

    Eigen::Matrix<double, N, 1> operator()(double t) const {
      try {
        return valuemap(t);
      } catch (Valuemap_out_of_range &e) {
        gthrow(
            {"Out-of-range error in boundary values!\n",
             std::string(e.what())});
      }
    }

  private:
    Valuemap<N> const valuemap;

  }; // namespace Model::Networkproblem
} // namespace Model::Networkproblem
