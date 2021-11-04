#pragma once
#include "Constraintcomponent.hpp"
#include "Flowboundarynode.hpp"
#include "SimpleConstraintcomponent.hpp"

namespace Model::Gas {

  class ConstraintSink final :
      public Flowboundarynode,
      public SimpleConstraintcomponent {

  public:
    static nlohmann::json get_constraint_schema();
    static std::string get_type();

    ConstraintSink(nlohmann::json const &data);

    void evaluate_constraint(
        Eigen::Ref<Eigen::VectorXd> constraint_values, double time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void d_evaluate_constraint_d_state(
        Aux::Matrixhandler &constraint_new_state_jacobian_handler, double time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void d_evaluate_constraint_d_control(
        Aux::Matrixhandler &constraint_control_jacobian_handler, double time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const final;

    void set_constraint_lower_bounds(
        Aux::InterpolatingVector &full_control_vector,
        const nlohmann::json &constraint_lower_bounds_json) const final;

    void set_constraint_upper_bounds(
        Aux::InterpolatingVector &full_control_vector,
        const nlohmann::json &constraint_upper_bounds_json) const final;

    Eigen::Index needed_number_of_constraints_per_time_point() const final;
  };

} // namespace Model::Gas
