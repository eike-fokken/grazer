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
    void evaluate_point_internal(
        Eigen::Ref<Eigen::Vector<double, Dimension>> rootvalues_segment,
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_right,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_right,
        Model::Balancelaw::Balancelaw<Dimension> const &bl) const final {

      double Delta_t = new_time - last_time;
      rootvalues_segment
          = 0.5 * (new_left + new_right) - 0.5 * (last_left + last_right)
            - Delta_t / Delta_x * (bl.flux(new_left) - bl.flux(new_right))
            - 0.5 * Delta_t * (bl.source(new_right) + bl.source(new_left));
    }

    void evaluate_point(
        Eigen::Index current_equation_index, double Delta_x,
        Balancelaw::Balancelaw<Dimension> const &balance_law,
        Eigen::Ref<Eigen::Vector<double, Dimension>> rootvalues,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final {

      auto rootvalue_segment
          = rootvalues.template segment<Dimension>(current_equation_index);

      auto last_left = last_state.segment<Dimension>(current_equation_index);
      auto last_right
          = last_state.segment<Dimension>(current_equation_index + 2);
      auto new_left = new_state.segment<Dimension>(current_equation_index);
      auto new_right = new_state.segment<Dimension>(current_equation_index + 2);

      evaluate_point_internal(
          rootvalue_segment, last_time, new_time, Delta_x, last_left,
          last_right, new_left, new_right, balance_law);
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

    void d_evaluate_point_d_new_state(
        Eigen::Index current_equation_index, double Delta_x,
        Balancelaw::Balancelaw<Dimension> const &balance_law,
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final {
      // maybe use Eigen::Ref here to avoid copies.
      auto last_left = last_state.segment<Dimension>(current_equation_index);
      auto last_right
          = last_state.segment<Dimension>(current_equation_index + Dimension);
      auto new_left = new_state.segment<Dimension>(current_equation_index);
      auto new_right
          = new_state.segment<Dimension>(current_equation_index + Dimension);

      Eigen::Matrix<double, Dimension, Dimension> current_derivative_left
          = devaluate_point_d_new_left(
              last_time, new_time, Delta_x, last_left, last_right, new_left,
              new_right, balance_law);

      Eigen::Matrix<double, Dimension, Dimension> current_derivative_right
          = devaluate_point_d_new_right(
              last_time, new_time, Delta_x, last_left, last_right, new_left,
              new_right, balance_law);

      for (auto j = 0; j != Dimension; ++j) {
        for (auto k = 0; k != Dimension; ++k) {
          jacobianhandler.add_to_coefficient(
              current_equation_index + j, current_equation_index + k,
              current_derivative_left(j, k));
          jacobianhandler.add_to_coefficient(
              current_equation_index + j,
              current_equation_index + Dimension + k,
              current_derivative_right(j, k));
        }
      }
    }
    void d_evaluate_point_d_last_state(
        Eigen::Index current_equation_index, double Delta_x,
        Balancelaw::Balancelaw<Dimension> const &balance_law,
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final {

      auto last_left = last_state.segment<Dimension>(current_equation_index);
      auto last_right
          = last_state.segment<Dimension>(current_equation_index + Dimension);
      auto new_left = new_state.segment<Dimension>(current_equation_index);
      auto new_right
          = new_state.segment<Dimension>(current_equation_index + Dimension);

      Eigen::Matrix<double, Dimension, Dimension> current_derivative_left
          = devaluate_point_d_last_left(
              last_time, new_time, Delta_x, last_left, last_right, new_left,
              new_right, balance_law);

      Eigen::Matrix<double, Dimension, Dimension> current_derivative_right
          = devaluate_point_d_last_right(
              last_time, new_time, Delta_x, last_left, last_right, new_left,
              new_right, balance_law);

      for (auto j = 0; j != Dimension; ++j) {
        for (auto k = 0; k != Dimension; ++k) {
          jacobianhandler.add_to_coefficient(
              current_equation_index + j, current_equation_index + k,
              current_derivative_left(j, k));
          jacobianhandler.add_to_coefficient(
              current_equation_index + j,
              current_equation_index + Dimension + k,
              current_derivative_right(j, k));
        }
      }
    }
  };

  Implicitboxscheme<2> schema;
} // namespace Model::Scheme
