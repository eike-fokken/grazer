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
#include "Balancelaw.hpp"
#include "Threepointscheme.hpp"
#include <Eigen/Sparse>

namespace Model::Scheme {

  template <int Dimension>
  class Implicitboxscheme : public Threepointscheme<Dimension> {
  public:
    ~Implicitboxscheme() {}
    /// Computes the implicit box scheme at one point.
    void evaluate_point(
        Eigen::Ref<Eigen::Vector<double, Dimension>> result, double last_time,
        double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_right,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_right,
        Model::Balancelaw::Balancelaw<Dimension> const &bl) const final {

      double Delta_t = new_time - last_time;
      result = 0.5 * (new_left + new_right) - 0.5 * (last_left + last_right)
               - Delta_t / Delta_x * (bl.flux(new_left) - bl.flux(new_right))
               - 0.5 * Delta_t * (bl.source(new_right) + bl.source(new_left));
    }

    Eigen::Matrix<double, Dimension, Dimension> devaluate_point_d_new_left(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*last_left*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*last_right*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*new_right*/,
        Model::Balancelaw::Balancelaw<Dimension> const &bl) const final {
      double Delta_t = new_time - last_time;
      Eigen::Matrix<double, Dimension, Dimension> jac;
      Eigen::Matrix<double, Dimension, Dimension> id;
      id.setIdentity();
      jac = 0.5 * id - Delta_t / Delta_x * bl.dflux_dstate(new_left)
            - 0.5 * Delta_t * bl.dsource_dstate(new_left);
      return jac;
    }

    Eigen::Matrix<double, Dimension, Dimension> devaluate_point_d_new_right(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*last_left*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*last_right*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*new_left*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_right,
        Model::Balancelaw::Balancelaw<Dimension> const &bl) const final {

      double Delta_t = new_time - last_time;
      Eigen::Matrix<double, Dimension, Dimension> jac;
      Eigen::Matrix<double, Dimension, Dimension> id;
      id.setIdentity();
      jac = 0.5 * id + Delta_t / Delta_x * bl.dflux_dstate(new_right)
            - 0.5 * Delta_t * bl.dsource_dstate(new_right);
      return jac;
    }

    Eigen::Matrix<double, Dimension, Dimension> devaluate_point_d_last_left(
        double /*last_time*/, double /*new_time*/, double /*Delta_x*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*last_left*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*last_right*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*new_left*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*new_right*/,
        Model::Balancelaw::Balancelaw<Dimension> const & /*bl*/) const final {

      Eigen::Matrix<double, Dimension, Dimension> jac;
      Eigen::Matrix<double, Dimension, Dimension> id;
      id.setIdentity();
      jac = -0.5 * id;
      return jac;
    }

    Eigen::Matrix<double, Dimension, Dimension> devaluate_point_d_last_right(
        double /*last_time*/, double /*new_time*/, double /*Delta_x*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*last_left*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*last_right*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*new_left*/,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> /*new_right*/,
        Model::Balancelaw::Balancelaw<Dimension> const & /*bl*/) const final {

      Eigen::Matrix<double, Dimension, Dimension> jac;
      Eigen::Matrix<double, Dimension, Dimension> id;
      id.setIdentity();
      jac = -0.5 * id;
      return jac;
    }
  };
} // namespace Model::Scheme
