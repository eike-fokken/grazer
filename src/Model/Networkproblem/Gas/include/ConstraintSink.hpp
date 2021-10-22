#pragma once
#include "SimpleConstraintcomponent.hpp"
#include "Sink.hpp"

namespace Model::Gas {

  class ConstraintSink final : public Sink, public SimpleConstraintcomponent {

  public:
    static std::string get_type();

    using Sink::Sink;

    void evaluate_constraint(
        Eigen::Ref<Eigen::VectorXd> constraint_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evaluate_constraint_d_state(
        Aux::Matrixhandler &constraint_new_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void d_evaluate_constraint_d_control(
        Aux::Matrixhandler &constraint_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const override;

    void set_constraint_lower_bounds(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> constraint_lower_bounds,
        nlohmann::json const &constraint_lower_bound_json) override;

    void set_constraint_upper_bounds(
        Timedata timedata, Eigen::Ref<Eigen::VectorXd> constraint_upper_bounds,
        nlohmann::json const &constraint_upper_bound_json) override;

    Eigen::Index needed_number_of_constraints_per_time_point() const override;
  };

} // namespace Model::Gas
