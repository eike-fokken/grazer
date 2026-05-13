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
#include "TwoGasMixture.hpp"
#include "Exception.hpp"
#include "Mathfunctions.hpp"
#include "unit_conversion.hpp"
#include <Eigen/src/Core/Matrix.h>
#include <string>

namespace Model::Balancelaw {

  TwoGasMixture::TwoGasMixture(nlohmann::json const &json) :
      sigma1(json["sigma1_m_per_s"]), sigma2(json["sigma2_m_per_s"]) {}

  Eigen::Vector3d
  TwoGasMixture::flux(Eigen::Ref<Eigen::Vector3d const> state) const {

    Eigen::Vector3d flux;
    flux[0] = rho1(state) * u(state);
    flux[1] = rho2(state) * u(state);
    flux[2] = p(state) + q(state) * q(state) / rho(state);

    return flux;
  }

  Eigen::Matrix3d
  TwoGasMixture::dflux_dstate(Eigen::Ref<Eigen::Vector3d const> state) const {

    Eigen::Vector3d flux_ = flux(state);
    Eigen::Matrix3d dflux;

    auto q_over_rho = q(state) / rho(state);

    dflux.row(0)
        = 1 / rho(state) * Eigen::RowVector3d(flux_[1], -flux_[0], rho1(state));

    dflux.row(1)
        = 1 / rho(state) * Eigen::RowVector3d(-flux_[1], flux_[0], rho2(state));

    dflux.row(2) = Eigen::RowVector3d(
        sigma1 * sigma1 - q_over_rho * q_over_rho,
        sigma2 * sigma2 - q_over_rho * q_over_rho, 2 * q_over_rho);

    return dflux;
  }

  Eigen::Vector3d
  TwoGasMixture::source(Eigen::Ref<Eigen::Vector3d const> /*state*/) const {
    return Eigen::Vector3d::Zero();
  }

  Eigen::Matrix3d TwoGasMixture::dsource_dstate(
      Eigen::Ref<Eigen::Vector3d const> /*state*/) const {
    return Eigen::Matrix3d::Zero();
  }

  double TwoGasMixture::rho(Eigen::Ref<Eigen::Vector3d const> state) const {
    return rho1(state) + rho2(state);
  }
  double TwoGasMixture::rho1(Eigen::Ref<Eigen::Vector3d const> state) const {
    return state[0];
  }
  double TwoGasMixture::rho2(Eigen::Ref<Eigen::Vector3d const> state) const {
    return state[1];
  }
  double TwoGasMixture::q(Eigen::Ref<Eigen::Vector3d const> state) const {
    return state[2];
  }

  double TwoGasMixture::p(Eigen::Ref<Eigen::Vector3d const> state) const {
    return sigma1 * sigma1 * rho1(state) + sigma2 * sigma2 * rho2(state);
  }

  double TwoGasMixture::u(Eigen::Ref<Eigen::Vector3d const> state) const {
    double const r = rho(state);
    if (r <= 0) {
      gthrow(
          {"The total gas density >>", std::to_string(r),
           "<< is non-positive, this case cannot be "
           "handled!"});
    }
    return q(state) / r;
  }

  Eigen::RowVector3d TwoGasMixture::drho_dstate(
      Eigen::Ref<Eigen::Vector3d const> /*state*/) const {
    return Eigen::RowVector3d(1.0, 1.0, 0.0);
  }

  Eigen::RowVector3d TwoGasMixture::drho1_dstate(
      Eigen::Ref<Eigen::Vector3d const> /*state*/) const {
    return Eigen::RowVector3d(1.0, 0.0, 0.0);
  }

  Eigen::RowVector3d TwoGasMixture::drho2_dstate(
      Eigen::Ref<Eigen::Vector3d const> /*state*/) const {
    return Eigen::RowVector3d(0.0, 1.0, 0.0);
  }

  Eigen::RowVector3d
  TwoGasMixture::dq_dstate(Eigen::Ref<Eigen::Vector3d const> /*state*/) const {
    return Eigen::RowVector3d(0.0, 0.0, 1.0);
  }

  Eigen::RowVector3d
  TwoGasMixture::dp_dstate(Eigen::Ref<Eigen::Vector3d const> /*state*/) const {
    return Eigen::RowVector3d(sigma1 * sigma1, sigma2 * sigma2, 0.0);
  }

  Eigen::RowVector3d
  TwoGasMixture::du_dstate(Eigen::Ref<Eigen::Vector3d const> state) const {
    double const r = rho(state);

    if (r <= 0) {
      gthrow(
          {"The total gas density is non-positive, this case cannot be "
           "handled!"});
    }
    return dq_dstate(state) / r - u(state) * drho_dstate(state) / r;
  }

} // namespace Model::Balancelaw
