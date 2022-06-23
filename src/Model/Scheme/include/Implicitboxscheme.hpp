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
#include "Threepointscheme.hpp"
#include <Eigen/Sparse>

namespace Model::Balancelaw {
  class Pipe_Balancelaw;
} // namespace Model::Balancelaw

namespace Model::Scheme {

  class Implicitboxscheme : public Threepointscheme {
  public:
    ~Implicitboxscheme() {}
    /// Computes the implicit box scheme at one point.
    void evaluate_point(
        Eigen::Ref<Eigen::Vector2d> result, double last_time, double new_time,
        double Delta_x, Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const final;

    Eigen::Matrix2d devaluate_point_d_new_left(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const final;

    Eigen::Matrix2d devaluate_point_d_new_right(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const final;

    Eigen::Matrix2d devaluate_point_d_last_left(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const final;

    Eigen::Matrix2d devaluate_point_d_last_right(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector2d const> last_left,
        Eigen::Ref<Eigen::Vector2d const> last_right,
        Eigen::Ref<Eigen::Vector2d const> new_left,
        Eigen::Ref<Eigen::Vector2d const> new_right,
        Model::Balancelaw::Pipe_Balancelaw const &bl) const final;
  };
} // namespace Model::Scheme
