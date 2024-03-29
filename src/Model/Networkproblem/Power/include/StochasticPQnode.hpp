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
#include "Normaldistribution.hpp"
#include "Powernode.hpp"
#include <limits>

static_assert(
    std::numeric_limits<double>::has_signaling_NaN == true,
    "StochasticPQnode uses signaling_NaN. If you don't have that, replace the "
    "initialization of current_P and current_Q by something else.");

namespace Model::Power {

  class StochasticPQnode final : public Powernode {
  public:
    static std::string get_type();
    std::string get_power_type() const final;

    static nlohmann::json get_schema();
    static nlohmann::json get_boundary_schema();

    void setup() final;

    StochasticPQnode(nlohmann::json const &topology);

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void d_evaluate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;

    void d_evaluate_d_last_state(
        Aux::Matrixhandler & /*jacobianhandler*/, double /*last_time*/,
        double /*new_time*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*last_state*/,
        Eigen::Ref<Eigen::VectorXd const> const & /*new_state*/) const final;

    void prepare_timestep(
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state) final;

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) final;

    // For testing purposes:
    double get_current_P() const;
    double get_current_Q() const;

  private:
    struct StochasticData {
      StochasticData(
          double _stability_parameter, double _cut_off_factor, double _sigma_P,
          double _theta_P, double _sigma_Q, double _theta_Q,
          int _number_of_stochastic_steps,
          std::array<uint32_t, Aux::pcg_seed_count> &used_seed) :
          distribution(used_seed),
          stability_parameter(_stability_parameter),
          cut_off_factor(_cut_off_factor),
          sigma_P(_sigma_P),
          theta_P(_theta_P),
          sigma_Q(_sigma_Q),
          theta_Q(_theta_Q),
          number_of_stochastic_steps(_number_of_stochastic_steps) {}

      StochasticData(
          double _stability_parameter, double _cut_off_factor, double _sigma_P,
          double _theta_P, double _sigma_Q, double _theta_Q,
          int _number_of_stochastic_steps,
          std::array<uint32_t, Aux::pcg_seed_count> &used_seed,
          std::array<uint32_t, Aux::pcg_seed_count> seed) :
          distribution(used_seed, seed),
          stability_parameter(_stability_parameter),
          cut_off_factor(_cut_off_factor),
          sigma_P(_sigma_P),
          theta_P(_theta_P),
          sigma_Q(_sigma_Q),
          theta_Q(_theta_Q),
          number_of_stochastic_steps(_number_of_stochastic_steps) {}

      Aux::Normaldistribution distribution;
      double const stability_parameter;
      double const cut_off_factor;
      double const sigma_P;
      double const theta_P;
      double const sigma_Q;
      double const theta_Q;

      int const number_of_stochastic_steps;
    };
    double current_P{std::numeric_limits<double>::signaling_NaN()};
    double current_Q{std::numeric_limits<double>::signaling_NaN()};
    std::unique_ptr<StochasticData> stochasticdata;
  };
} // namespace Model::Power
