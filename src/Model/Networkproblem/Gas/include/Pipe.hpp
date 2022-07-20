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

namespace Model::Balancelaw {
  class Pipe_Balancelaw;
}

namespace Model::Scheme {
  class Threepointscheme;
}

namespace Model::Gas {

  class Pipe final :
      public Equationcomponent,
      public Gasedge,
      public Network::Edge {
  public:
    static std::string get_type();
    std::string get_gas_type() const final;
    static int init_vals_per_interpol_point();
    static nlohmann::json get_schema();
    static nlohmann::json get_initial_schema();

    Pipe(
        nlohmann::json const &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    ~Pipe() final;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;
    void d_evaluate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;
    void d_evaluate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const final;
    void setup() final;

    Eigen::Index needed_number_of_states() const final;

    void add_results_to_json(nlohmann::json &new_output) final;

    void json_save(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state) final;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) const final;

    Eigen::Vector2d get_boundary_p_qvol_bar(
        Direction direction,
        Eigen::Ref<Eigen::VectorXd const> const &state) const final;

    void dboundary_p_qvol_dstate(
        Direction direction, Aux::Matrixhandler &jacobianhandler,
        Eigen::RowVector2d function_derivative, Eigen::Index rootvalues_index,
        Eigen::Ref<Eigen::VectorXd const> const &state) const final;

    Balancelaw::Pipe_Balancelaw const *get_balancelaw() const;

    int get_number_of_points() const;
    double get_Delta_x() const;

  private:
    double get_length() const;
    int const number_of_points;
    double const Delta_x;

    std::unique_ptr<Balancelaw::Pipe_Balancelaw const> balancelaw;
    std::unique_ptr<Scheme::Threepointscheme const> scheme;
  };

} // namespace Model::Gas
