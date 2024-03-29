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
#include "Boundaryvaluecomponent.hpp"
#include "Equationcomponent.hpp"
#include "InterpolatingVector.hpp"
#include "Node.hpp"
#include "SimpleStatecomponent.hpp"
#include <Eigen/Sparse>

namespace Model::Power {

  class Powernode :
      public Equationcomponent,
      public SimpleStatecomponent,
      public Network::Node,
      public Boundaryvaluecomponent {

  public:
    virtual std::string get_power_type() const = 0;
    static nlohmann::json get_schema();
    static nlohmann::json get_boundary_schema();
    static nlohmann::json get_initial_schema();

    Powernode(nlohmann::json const &topology);
    ~Powernode(){};

    double get_G() const;
    double get_B() const;

    void add_results_to_json(nlohmann::json &new_output) final;

    Eigen::Index needed_number_of_states() const final;

    void set_initial_values(
        Eigen::Ref<Eigen::VectorXd> new_state,
        nlohmann::json const &initial_json) const final;

    double P(Eigen::Ref<Eigen::VectorXd const> const &state) const;
    double Q(Eigen::Ref<Eigen::VectorXd const> const &state) const;
    void evaluate_P_derivative(
        Eigen::Index equationindex, Aux::Matrixhandler &jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const;
    void evaluate_Q_derivative(
        Eigen::Index equationindex, Aux::Matrixhandler &jacobianhandler,
        Eigen::Ref<Eigen::VectorXd const> const &new_state) const;

  protected:
    void setup_helper();
    void json_save_power(
        double time, Eigen::Ref<Eigen::VectorXd const> const &state,
        double P_val, double Q_val);

    Aux::InterpolatingVector const boundaryvalue;
    /// Real part of the admittance of this node
    double G;
    /// Imaginary part of the admittance of this node
    double B;

  private:
    /// \brief number of state variables, this component needs.
    static constexpr Eigen::Index number_of_state_variables{2};

    /// \brief Holds data on all attached transmission lines.
    std::vector<std::tuple<double, double, Powernode *>>
        attached_component_data;
    // TODO: Should be replaced by a struct {double line_G, double line_B, ... *
    // node}
  };

} // namespace Model::Power
