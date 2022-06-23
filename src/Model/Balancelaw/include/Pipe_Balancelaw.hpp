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
#include <Eigen/Dense>

namespace Model::Balancelaw {

  class Pipe_Balancelaw {

  public:
    Pipe_Balancelaw(){};

    virtual ~Pipe_Balancelaw() {}
    virtual Eigen::Vector2d
    flux(Eigen::Ref<Eigen::Vector2d const> state) const = 0;
    virtual Eigen::Matrix2d
    dflux_dstate(Eigen::Ref<Eigen::Vector2d const> state) const = 0;

    virtual Eigen::Vector2d
    source(Eigen::Ref<Eigen::Vector2d const> state) const = 0;
    virtual Eigen::Matrix2d
    dsource_dstate(Eigen::Ref<Eigen::Vector2d const> state) const = 0;

    virtual Eigen::Vector2d
    p_qvol(Eigen::Ref<Eigen::Vector2d const> state) const = 0;
    virtual Eigen::Matrix2d
    dp_qvol_dstate(Eigen::Ref<Eigen::Vector2d const> state) const = 0;

    virtual Eigen::Vector2d
    state(Eigen::Ref<Eigen::Vector2d const> p_qvol) const = 0;
    virtual double p_pascal_from_p_bar(double p) const = 0;
    virtual Eigen::Vector2d p_qvol_from_p_qvol_bar(
        Eigen::Ref<Eigen::Vector2d const> p_qvol_bar) const = 0;

    virtual double p_bar_from_p_pascal(double p) const = 0;
    virtual double dp_bar_from_p_pascal_dp_pascal(double p) const = 0;

    virtual Eigen::Vector2d
    p_qvol_bar_from_p_qvol(Eigen::Ref<Eigen::Vector2d const> p_qvol) const = 0;
    virtual Eigen::Matrix2d dp_qvol_bar_from_p_qvold_p_qvol(
        Eigen::Ref<Eigen::Vector2d const> p_qvol) const = 0;

    virtual double p(double rho) const = 0;
    virtual double dp_drho(double rho) const = 0;
    virtual double rho(double rho) const = 0;
  };
} // namespace Model::Balancelaw
