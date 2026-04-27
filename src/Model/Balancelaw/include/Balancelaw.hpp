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
#include <Eigen/src/Core/util/Constants.h>

namespace Model::Balancelaw {

  template <int Dimension> class Balancelaw {

  public:
    Balancelaw() {};

    virtual ~Balancelaw() {}
    virtual Eigen::Vector<double, Dimension>
    flux(Eigen::Ref<Eigen::Vector<double, Dimension> const> state) const = 0;

    virtual Eigen::Matrix<double, Dimension, Dimension>
    dflux_dstate(Eigen::Ref<Eigen::Vector<double, Dimension> const> state) const
        = 0;

    virtual Eigen::Vector<double, Dimension>
    source(Eigen::Ref<Eigen::Vector<double, Dimension> const> state) const = 0;
    virtual Eigen::Matrix<double, Dimension, Dimension> dsource_dstate(
        Eigen::Ref<Eigen::Vector<double, Dimension> const> state) const
        = 0;
  };
} // namespace Model::Balancelaw
