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
#include <Eigen/Dense>
#include <nlohmann/json.hpp>

namespace Model::Balancelaw {

  class TwoGasMixture : public Balancelaw<3> {

  public:
    TwoGasMixture(nlohmann::json const &json);

    Eigen::Vector3d flux(Eigen::Ref<Eigen::Vector3d const> state) const final;
    Eigen::Matrix3d
    dflux_dstate(Eigen::Ref<Eigen::Vector3d const> state) const final;

    Eigen::Vector3d source(Eigen::Ref<Eigen::Vector3d const> state) const final;
    Eigen::Matrix3d
    dsource_dstate(Eigen::Ref<Eigen::Vector3d const> state) const final;

    Eigen::Vector3d
    eigen_values(Eigen::Ref<Eigen::Vector3d const> state) const final;

    double lambda_1(Eigen::Ref<Eigen::Vector3d const> state) const;
    double lambda_2(Eigen::Ref<Eigen::Vector3d const> state) const;
    double lambda_3(Eigen::Ref<Eigen::Vector3d const> state) const;

    std::array<Eigen::Vector3d, 3>
    eigen_vectors(Eigen::Ref<Eigen::Vector3d const> state) const final;

    /** \brief Gas pressure.
     */
    [[nodiscard]] double p(Eigen::Ref<Eigen::Vector3d const> state) const;

    /** \brief Total density of the gas, sum of the component densities.
     */
    [[nodiscard]] double rho(Eigen::Ref<Eigen::Vector3d const> state) const;

    /** \brief Density of the first gas component.
     */
    [[nodiscard]] double rho1(Eigen::Ref<Eigen::Vector3d const> state) const;

    /** \brief Density of the second gas component.
     */
    [[nodiscard]] double rho2(Eigen::Ref<Eigen::Vector3d const> state) const;

    /** \brief Density share of the first gas component.
     */
    [[nodiscard]] double
    component_share1(Eigen::Ref<Eigen::Vector3d const> state) const;

    /** \brief Density share of the first gas component.
     */
    [[nodiscard]] double
    component_share2(Eigen::Ref<Eigen::Vector3d const> state) const;

    /** \brief Total gas flow.
     */
    [[nodiscard]] double q(Eigen::Ref<Eigen::Vector3d const> state) const;

    /** \brief Flow part of constituent 1.
     */
    [[nodiscard]] double q1(Eigen::Ref<Eigen::Vector3d const> state) const;

    /** \brief Flow part of constituent 2.
     */
    [[nodiscard]] double q2(Eigen::Ref<Eigen::Vector3d const> state) const;

    /** \brief The gas velocity. Is equal for both components.
     */
    [[nodiscard]] double u(Eigen::Ref<Eigen::Vector3d const> state) const;

    [[nodiscard]] Eigen::RowVector3d
    drho_dstate(Eigen::Ref<Eigen::Vector3d const> state) const;

    [[nodiscard]] Eigen::RowVector3d
    drho1_dstate(Eigen::Ref<Eigen::Vector3d const> state) const;

    [[nodiscard]] Eigen::RowVector3d
    drho2_dstate(Eigen::Ref<Eigen::Vector3d const> state) const;

    [[nodiscard]] Eigen::RowVector3d
    dcomponent_share1_dstate(Eigen::Ref<Eigen::Vector3d const> state) const;

    [[nodiscard]] Eigen::RowVector3d
    dcomponent_share2_dstate(Eigen::Ref<Eigen::Vector3d const> state) const;

    [[nodiscard]] Eigen::RowVector3d
    dq_dstate(Eigen::Ref<Eigen::Vector3d const> state) const;

    [[nodiscard]] Eigen::RowVector3d
    dp_dstate(Eigen::Ref<Eigen::Vector3d const> state) const;

    [[nodiscard]] Eigen::RowVector3d
    du_dstate(Eigen::Ref<Eigen::Vector3d const> state) const;

  private:
    double const sigma1;
    double const sigma2;
  };
} // namespace Model::Balancelaw
