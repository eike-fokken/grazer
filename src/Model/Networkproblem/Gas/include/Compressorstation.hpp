#pragma once
#include "Edge.hpp"
#include "Gasedge.hpp"
#include "Shortcomponent.hpp"
#include "SimpleControlcomponent.hpp"
#include <nlohmann/json.hpp>

namespace Model::Gas {

  class Compressorstation final :
      public SimpleControlcomponent,
      public Shortcomponent {
  public:
    static std::string get_type();
    std::string get_gas_type() const final;

    static nlohmann::json get_control_schema();

    Compressorstation(
        nlohmann::json const &edge_json,
        std::vector<std::unique_ptr<Network::Node>> &nodes);

    void setup() final;

    void evaluate(
        Eigen::Ref<Eigen::VectorXd> rootvalues, double last_time,
        double new_time, const Eigen::Ref<const Eigen::VectorXd> &last_state,
        const Eigen::Ref<const Eigen::VectorXd> &new_state,
        const Eigen::Ref<const Eigen::VectorXd> &control) const final;

    void d_evalutate_d_new_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        const Eigen::Ref<const Eigen::VectorXd> &last_state,
        const Eigen::Ref<const Eigen::VectorXd> &new_state,
        const Eigen::Ref<const Eigen::VectorXd> &control) const final;

    void d_evalutate_d_last_state(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        const Eigen::Ref<const Eigen::VectorXd> &last_state,
        const Eigen::Ref<const Eigen::VectorXd> &new_state,
        const Eigen::Ref<const Eigen::VectorXd> &control) const final;

    void d_evalutate_d_control(
        Aux::Matrixhandler &jacobianhandler, double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &last_state,
        Eigen::Ref<Eigen::VectorXd const> const &new_state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void set_initial_controls(
        Aux::InterpolatingVector &full_control_vector,
        nlohmann::json const &control_json) const final;

    void set_lower_bounds(
        Aux::InterpolatingVector &full_lower_bound_vector,
        nlohmann::json const &lower_bound_json) const final;

    void set_upper_bounds(
        Aux::InterpolatingVector &full_upper_bound_vector,
        nlohmann::json const &upper_bound_json) const final;

    Eigen::Index needed_number_of_controls_per_time_point() const final;

    void add_results_to_json(nlohmann::json &new_output) final;
  };

} // namespace Model::Gas
