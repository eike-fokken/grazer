#pragma once
#include "Eigen/Sparse"
#include <nlohmann/json.hpp>

namespace Aux {

  class Matrixhandler;
}

namespace Model {
  class SimpleConstraintcomponent;

  class Constraintcomponent {
    /** \brief SimpleConstraintcomponent is a friend of Constraintcomponent to
     * give it access to #start_constraint_index and #after_constraint_index.
     */
    friend class SimpleConstraintcomponent;
    friend class Networkproblem;

  public:
    virtual ~Constraintcomponent(){};

    virtual void evaluate_constraint(
        Eigen::Ref<Eigen::VectorXd> constraint_values, double last_time,
        double new_time, Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    virtual void d_evaluate_constraint_d_state(
        Aux::Matrixhandler &constraint_new_state_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    virtual void d_evaluate_constraint_d_control(
        Aux::Matrixhandler &constraint_control_jacobian_handler,
        double last_time, double new_time,
        Eigen::Ref<Eigen::VectorXd const> const &state,
        Eigen::Ref<Eigen::VectorXd const> const &control) const = 0;

    /** \brief This function sets the indices #start_constraint_index and
     * #after_constraint_index.
     *
     * @param next_free_index The first constraint index that is currently
     * not claimed by another component.
     * @returns The new lowest free index.
     */
    virtual int set_constraint_indices(int const next_free_index) = 0;

    int get_number_of_inequalities_per_timepoint() const;

    /** \brief getter for #start_constraint_index
     */
    int get_start_constraint_index() const;

    /** \brief getter for #after_constraint_index
     */
    int get_after_constraint_index() const;

    virtual void set_constraint_lower_bounds(
        double start_time, double end_time, int number_of_time_steps,
        Eigen::Ref<Eigen::VectorXd> constraint_lower_bounds,
        nlohmann::json const &constraint_lower_bound_json)
        = 0;

    virtual void set_constraint_upper_bounds(
        double start_time, double end_time, int number_of_time_steps,
        Eigen::Ref<Eigen::VectorXd> constraint_upper_bounds,
        nlohmann::json const &constraint_upper_bound_json)
        = 0;

  private:
    /** \brief The first index, this Constraintcomponent "owns".
     */
    int start_constraint_index{-1};

    /** \brief The first index after #start_constraint_index, that is not
     * "owned" by this Constraintcomponent.
     */
    int after_constraint_index{-1};
  };
} // namespace Model
