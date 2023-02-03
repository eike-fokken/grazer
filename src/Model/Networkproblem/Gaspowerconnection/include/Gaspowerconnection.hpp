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
#include "Edge.hpp"
#include "Equationcomponent.hpp"
#include "Gasedge.hpp"
#include "InterpolatingVector.hpp"

namespace Model::Gaspowerconnection {

  class ExternalPowerplant;
  /** \brief an edge connecting a gasnode to a powernode and modelling a
   * gaspower plant/power-to-gas plant.
   *
   * The plant can be used in  gas-controlled mode and power-controlled mode.
   * In gas-controlled mode, it uses its boundary values as a set pressure to be
   * maintained at its inlet and converts all resulting flow into power.
   *
   * In power-controlled mode, it doesn't enforce the pressure. In this case an
   * additional equation must be set in the powernode at the connections end.
   */
  class Gaspowerconnection final :
      public Equationcomponent,
      public Gas::Gasedge,
      public Network::Edge {
  public:
    static std::string get_type();
    std::string get_gas_type() const final;
    static nlohmann::json get_schema();
    static nlohmann::json get_initial_schema();

    Gaspowerconnection(
        nlohmann::json const &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const final;
    void d_evaluate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const final;

    void d_evaluate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const & /*control*/) const final;

    void setup() final;

    Eigen::Index needed_number_of_states() const final;

    void add_results_to_json(nlohmann::json &new_output) final;

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) final;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) const final;

    Eigen::Vector2d get_boundary_p_qvol_bar(
        Gas::Direction direction,
        Eigen::Ref<Eigen::VectorXd const> const &state) const final;

    void dboundary_p_qvol_dstate(
        Gas::Direction direction, Aux::Matrixhandler &jacobianhandler,
        Eigen::RowVector2d function_derivative, Eigen::Index rootvalues_index,
        Eigen::Ref<Eigen::VectorXd const> const &state) const final;

    double smoothing_polynomial(double q) const;
    double dsmoothing_polynomial_dq(double q) const;

    double generated_power(double q) const;
    double dgenerated_power_dq(double q) const;

    static constexpr double kappa{60};

  private:
    ExternalPowerplant *powerendnode{nullptr};

    double const gas2power_q_coefficient;
    double const power2gas_q_coefficient;
  };
} // namespace Model::Gaspowerconnection
