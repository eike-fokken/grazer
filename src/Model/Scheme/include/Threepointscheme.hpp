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
#include <Eigen/Sparse>

namespace Model::Scheme {

  template <int Dimension> class Threepointscheme {

  public:
    virtual ~Threepointscheme() {}

    /// Computes the scheme at one point.
    virtual void evaluate_point(
        Eigen::Ref<Eigen::Vector<double, Dimension>> result, double last_time,
        double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_right,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_right,
        Model::Balancelaw::Balancelaw<Dimension> const &bl) const
        = 0;

    /// The derivative with respect to \code{.cpp}new_left\endcode
    virtual Eigen::Matrix<double, Dimension, Dimension>
    devaluate_point_d_new_left(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_right,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_right,
        Model::Balancelaw::Balancelaw<Dimension> const &bl) const
        = 0;

    /// \brief The derivative with respect to \code{.cpp}new_right\endcode
    virtual Eigen::Matrix<double, Dimension, Dimension>
    devaluate_point_d_new_right(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_right,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_right,
        Model::Balancelaw::Balancelaw<Dimension> const &bl) const
        = 0;

    /// The derivative with respect to \code{.cpp}last_left\endcode
    virtual Eigen::Matrix<double, Dimension, Dimension>
    devaluate_point_d_last_left(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_right,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_right,
        Model::Balancelaw::Balancelaw<Dimension> const &bl) const
        = 0;

    /// \brief The derivative with respect to \code{.cpp}last_right\endcode
    virtual Eigen::Matrix<double, Dimension, Dimension>
    devaluate_point_d_last_right(
        double last_time, double new_time, double Delta_x,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> last_right,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_left,
        Eigen::Ref<Eigen::Vector<double, Dimension> const> new_right,
        Model::Balancelaw::Balancelaw<Dimension> const &bl) const
        = 0;
  };
} // namespace Model::Scheme
