/*
 * Grazer - network simulation tool
 *
 * Copyright 2020-2021 Uni Mannheim <e.fokken+grazer@posteo.de>,
 *
 * SPDX-License-Identifier:	AGPL-3.0-or-later
 *
 * Licensed under the GNU Affero General Public License v3.0, found in
 * LICENSE.txt and at https://www.gnu.org/licenses/agpl-3.0.html
 * This file may not be copied, modified, or distributed except according to
 * those terms.
 *
 * Distributed on an "AS IS" BASIS, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied.  See your chosen license for details.
 *
 */

#pragma once
#include "Edge.hpp"
#include "Gasedge.hpp"

namespace Model::Balancelaw {
  class Pipe_Balancelaw;
}

namespace Model::Scheme {
  class Threepointscheme;
}

namespace Model::Networkproblem::Gas {

  class Pipe final : public Gasedge, public Network::Edge {
  public:
    static std::string get_type();
    std::string get_gas_type() const override;
    static int init_vals_per_interpol_point();
    static nlohmann::json get_schema();
    static nlohmann::json get_initial_schema();

    Pipe(
        nlohmann::json const &topology,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    ~Pipe() override;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> last_state,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;
    void evaluate_state_derivative(
        Aux::Matrixhandler *jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const>,
        Eigen::Ref<Eigen::VectorXd const> new_state) const override;

    void setup() override;

    int get_number_of_states() const override;

    void print_to_files(nlohmann::json &new_output) override;

    void
    json_save(double time, Eigen::Ref<const Eigen::VectorXd> state) override;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) override;

    Eigen::Vector2d get_boundary_p_qvol_bar(
        int direction, Eigen::Ref<Eigen::VectorXd const> state) const override;

    void dboundary_p_qvol_dstate(
        int direction, Aux::Matrixhandler *jacobianhandler,
        Eigen::RowVector2d function_derivative, int rootvalues_index,
        Eigen::Ref<Eigen::VectorXd const> state) const override;

  private:
    double get_length();
    int const number_of_points;
    double const Delta_x;

    std::unique_ptr<Balancelaw::Pipe_Balancelaw const> bl;
    std::unique_ptr<Scheme::Threepointscheme const> scheme;
  };

} // namespace Model::Networkproblem::Gas
